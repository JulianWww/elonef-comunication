import { randomBytes } from "crypto";
import { PrivateClientKey, SignedKey } from "../keys/genKeys";
import { ConnectionHandler } from "./connectionHandler";
import { WebSocket, MessageEvent } from "ws";
import { import_private, import_public } from "../keys";
import { sign, sign_nonstreamable, verifyAndRaiseError } from "../encription/sign";
import { BufferReader, bufferArrayToBuffer, bufferToBufferArray, bufferToNumber, bufferToString, bufferToStringArray, extractDynamicBuffer, numberToBuffer, shortNumberToBuffer, stringToBuffer, uuid_size } from "../encoding";
import { decript_aes, decript_rsa, encript_aes, encript_rsa } from "../encription";
import { toKeyMapWithValue, to_byte } from "../utility";
import { Buffer } from "buffer";
import forge from "node-forge";

import { Message, byte } from "../types";


/**
 * a connection handler that will connect to a server running the serverConnectionHandler. It will connect and automatically authenticate the user with the private key provided. It provides the ability to send and receive messages for any chat you are a part of.
 * Furhtermore there is the possibility to send and recive api requests to and from the server and create a new chat key for a given user. Any authentication of these changes must be done by the server side backend. To bind api points on the client use the add_api_callback method.
 */
export class ClientConnectionHandler extends ConnectionHandler {
    /**
     * the websocket connection to the server.
     */
    private sock: WebSocket;

    /**
     * the private key used to authenticate the user and sign data.
     */
    private sign_key: Buffer;
    /**
     * the public key used to encrypt data.
     */
    private data_key: forge.pki.rsa.PrivateKey;
    /**
     * the UUID of the user (not a name but an id).
     */
    private uid: string;

    /**
     * a promise that is resolved when the user is authenticated.
     */
    private ready: Promise<void>;
    /**
     * the resolver to the ready promise.
     */
    private make_ready: VoidFunction;

    /**
     * the cache of public keys used for optimization by avoiding asking the server for the public key for every message. This map is used by the get_keys method and keys are automatically added to the cache. All keys in the cache are verified, assuming we had verification witch is still WIP
     */
    private signature_keys: Map<string, Promise<Buffer | forge.pki.PublicKey>>;
    /**
     * an AES key map used to decript message contents.
     */
    private aes_keys: Map<string, Promise<Buffer>>;
    private buffer_getter: (data: any) => Promise<Buffer>


    /**
     * 
     * @param address the address of the server to connect to.
     * @param priv_key the private key used to authenticate the user, sign data and get data points.
     * @param error_handler a handler for errors. when an error occurs on a coll on the remote end, that is forwarded (aka is a ForwardedError) his function will be called before reject and if it returns true reject will be called without parameters.
     */
    constructor(sock: WebSocket, priv_key: PrivateClientKey, buffer_getter: (data: any) => Promise<Buffer> = async (v)=> v.data, error_handler: (err: string) => boolean = () => false) {
        super(error_handler);

        this.sign_key = import_private(priv_key.sign_key) as Buffer;
        this.data_key = import_private(priv_key.data_key) as forge.pki.rsa.PrivateKey;
        this.uid = priv_key.uid;
        
        this.sock = sock
        this.sock.addEventListener("message", this.get_data_from_server);

        this.make_ready = () => 1;
        this.ready = new Promise<void>((resolve, _)=> {
            this.make_ready = resolve;
        });

        this.signature_keys = new Map();
        this.aes_keys = new Map();
        this.buffer_getter = buffer_getter
    }

    /**
     * handle authentication a connection. This handles the client side of the authentication process. It signs a message buffer with the private sign key and sends it to the server.
     * @param ws dummy websocket.
     * @param userid dummy user id.
     * @param message the data the server wants to have signed
     * @param authenticated dummy if the connection is authenticated.
     * @returns the signature
     */
    protected async handle_auth(ws: WebSocket, userid: string, message: Buffer, authenticated: boolean): Promise<Buffer> {
        const out = Buffer.concat([
            stringToBuffer(this.uid),
            await sign_nonstreamable(message, this.sign_key)
        ]);
        return out;
    }

    /**
     * handles messges that should only be handled on the client side
     * @param ws the socket
     * @param userid dummy on the client
     * @param message the recived data
     * @param authenticated if the connection is authenticed
     * @param type type id of the message
     * @returns 
     */
    protected async handle_single_side_messages(ws: WebSocket, userid: string, message: Buffer, authenticated: boolean, type: number): Promise<Buffer | undefined> {
        if (type == 16) {
            this.make_ready()
        }
        
        return undefined
    }

    /**
     * internal holder tha waits for the connection to be finalized.
     */
    async wait_for_connection() {
        await this.ready;
    }

    /**
     * internal send some data to the server and wait for a responce. see super.send for more info.
     * @param ws the socket to send the data over.
     * @param message the data to be sent to the server.
     * @param type the type of the request. see the request type table in the super.send documentation for more info.
     * @param kill_on_unauth weather to kill unothorized requests automatically. false for the client.
     * @returns the data returned by the server.
     */
    protected async send(ws: WebSocket, message: string | Buffer, type: number, kill_on_unauth?: boolean): Promise<Buffer> {
        await this.wait_for_connection();
        return await super.send(ws, message, type, kill_on_unauth);
    }

    /**
     * handler that populates the static contents for handle_message. this is a callback for when the socket recieves a message from the server.
     * @param data the data recived by the sever
     */
    protected get_data_from_server = async (data: MessageEvent) => {
        this.handle_message(this.sock, "", await this.buffer_getter(data), true);
    }

    /**
     * Make a request to the servers api. The name of the api must be in the servers api callback table. Read the section on api in the server doc for more info.
     * @param request_id the name of the api bind point to call
     * @param data the data that should be sent to the api bind point.
     * @returns the data returned form the api call.
     */
    async make_api_request(request_id: Buffer | string, data: Buffer | string) {
        return this._make_api_request(this.sock, request_id, data);
    }

    /**
     * get ether the data of signature public key for some users. the id is 16 for data keys and 20 for signature keys. This function shold only be called from get_pub_keys or get_signature_key. 
     * The function checks the key cache to see if the key is already in the cache. If it is not in the cache it will ask the server for the key and add it to the cache.
     * It will also verify the keys once verification is implemented.
     * @param users the list of users whos key should be gotten
     * @param id the numeric id of the key. 16 for data keys and 20 for signature keys.
     * @returns the keys by user.
     */
    private async get_key(users: string[], id: number) {
        const unkown_users: [string, (key: Buffer | forge.pki.PublicKey)=>void][] = []

        // check for known keys and add the others to the unknown users list.
        const known_keys = await Promise.all(users
            .map((user) => [user, this.signature_keys.get(id + user)] as [string, Promise<Buffer | forge.pki.PublicKey>])
            .filter(([user, key]) => {
                if (!key) {
                    const promise = new Promise<Buffer | forge.pki.PublicKey>((resolve, reject) => {
                        unkown_users.push([user, resolve]);
                    })
                    this.signature_keys.set(id + user, promise);
                    
                    return false;
                }
                return true;
            })
            .map(async ([user, key]) => {
                return {
                    user: user,
                    key: await key
                }
            }));
        return toKeyMapWithValue(
            [
                ...known_keys,
                // request the unkown users from the server and perform en and decoding of the data.
                ...bufferToStringArray(
                    new BufferReader(
                        await this.send(
                            this.sock, 
                            bufferArrayToBuffer(
                                unkown_users.map(([user, resolve]) => user)
                            ), 
                            id
                        )
                    )
                )
                .map((key: string) => JSON.parse(key) as SignedKey)
                .map((key: SignedKey) => {
                    const keyobj = import_public(key.key);
                    if (!keyobj) {
                        throw Error("missing public key for a certain user");
                    }
                    return keyobj;
                })
                // add the keys to the cache and output them in a more usefull format.
                .map((key, idx: number) => {
                    const [user, resolve] = unkown_users[idx];
                    resolve(key);
                    return {
                        user,
                        key
                    }
                })
            ],
            "user", 
            "key"
        ) as Record<string, Buffer | forge.pki.PublicKey>;
    }

    /**
     * Gets the public data keys of a list of users. The keys will be verified once verification is implemented.
     * @param users the usesrs whos keys should be gotten.
     * @returns the user to key map.
     */
    private async get_pub_keys(users: string[]): Promise<Record<string, forge.pki.rsa.PublicKey>> {
        return (await this.get_key(users, 16)) as Record<string, forge.pki.rsa.PublicKey>;
    }

    /**
     * Gets the signature data keys of a list of users. The keys will be verified once verification is implemented.
     * @param user the usesr whos keys should be gotten.
     * @returns the user to key map.
     */
    async get_signature_key(user: string): Promise<Buffer> {
        return (await this.get_key([user], 20))[user] as Buffer
    }

    /**
     * generates a new chat key and sends it to the server. The server will sore the key untill it is needed. A chat key is an 32 byte AES key that is encripted with the public data key of every user in the chat. this allows them all to use the same key to encrypt and decrypt messages.
     * This function should be called when the chat is created and whenever the user roster changes.
     * @param users the list of users in the chat. 
     * @param the id of the chat to add this key to.
     */
    async generate_chat_keys(users: string[], chat_id: string) {
        const key = this.get_pub_keys(users);

        const aes_key = randomBytes(32);

        const enc = bufferArrayToBuffer(
            (Object.entries(await key)).map(
                ([name, key] : [string, forge.pki.rsa.PublicKey]) => Buffer.concat(
                    [
                        stringToBuffer(name),
                        encript_rsa(aes_key, key)
                    ]
                )
            )
        )
        await this.send(
            this.sock, 
            Buffer.concat([
                stringToBuffer(chat_id),
                enc
            ])
            , 17
        );
    }

    /**
     * gets the newest chat key from the server. Note: no cachin is used. This function is called whenever a new message is beeing prepared for sending.
     * @param chat_id the id of the chat to get the key for.
     * @returns the uuid and the AES key to use for encription.
     */
    private async get_newest_chat_key(chat_id: string) {
        const reader = new BufferReader(
            await this.send(
                this.sock, 
                stringToBuffer(chat_id)
                , 18
            )
        );

        if (!reader.canRead()) {
            throw new Error("no chat key provided by the server");
        }
        
        return {
            uuid: reader.read(uuid_size),
            key: decript_rsa(reader.readRest(), this.data_key)
        };
    }

    /**
     * get che AES key used to encript a certain message by its uuid.
     * @param chat_id the id of the chat to get the key for.
     * @param key_id the uuid of the key.
     * @returns the key
     */
    private async get_chat_key(chat_id: string, key_id: Buffer) {
        const id = key_id.toString("base64") + ":" + chat_id;
        var key = this.aes_keys.get(id);
        if (key) {
            return await key;
        }

        key = new Promise((resolve) => 
            this.send(
                this.sock,
                Buffer.concat([
                    key_id,
                    Buffer.from(chat_id)
                ]),
                22
            )
            .then((buf: Buffer) => {
                if (buf.length === 0) {
                    throw new Error("no chat key provided by the server");
                }
                return buf;
            })
            .then((buf: Buffer) => decript_rsa(buf, this.data_key))
            .then((buf: Buffer) => resolve(buf))
        );

        this.aes_keys.set(id, key);
        return await key;
    }

    /**
     * Send a message into the chat. the message will be end2end encrypted. using the chat key that is only known to memebers of the chat.
     * @param message the message to be sent
     * @param chat_id 
     */
    async send_message(message: Buffer, message_type: byte, chat_id: string) {
        const { uuid, key } = await this.get_newest_chat_key(chat_id);
        const enc = await sign(
            Buffer.concat([
                numberToBuffer(Date.now() / 1000),  // time sent
                to_byte(message_type),              // the message type byte
                uuid,                               // the decryption key uuid
                encript_aes(                        // the encrypted message
                    Buffer.from(
                        message
                    ), 
                    key
                )
                
            ]),
            this.sign_key
        );

        this.send(
            this.sock,
            Buffer.concat([
                stringToBuffer(chat_id),
                enc
            ]),
            19
        )
    }

    /**
     * laod and decode a certain amount of messages from the server statring with a reverse index. 
     * @param chat_id the id of the chat to load messages from.
     * @param back_idx the revese index of the last message that should be loaded.
     * @param length amount of messages that should be loaded.
     * @returns list of messages with user that sent them. the source is verified.
     */
    async get_message(chat_id: string, back_idx: number, length: number) {
        const messages = await Promise.all(
                bufferToBufferArray(
                new BufferReader(
                    await this.send(
                        this.sock,
                        Buffer.concat([
                            stringToBuffer(chat_id),
                            numberToBuffer(back_idx),
                            numberToBuffer(length)
                        ]),
                        21
                    )
                )
            )
            .map(
                (buf: Buffer) => new BufferReader(buf)
            )
            .map((reader: BufferReader) => {
                return {
                    sender_id: bufferToString(reader),
                    signed_block: extractDynamicBuffer(reader),
                    upload_time: bufferToNumber(reader),
                    reactions: reader.readRest()
                }
            })
            .map(async (data) => {
                const key = await this.get_signature_key(data.sender_id)

                const verified = new BufferReader(
                    await verifyAndRaiseError(
                        new BufferReader(
                            data.signed_block
                        ),
                        key
                    )
                );
                return {
                    ...data,
                    sent_time: bufferToNumber(verified),
                    msg_type: verified.read(1)[0],
                    aes_key_id: verified.read(uuid_size),
                    data: verified.readRest(),
                }
            })
            .map(async (data_promise) => {
                const data = await data_promise;
                const key = await this.get_chat_key(chat_id, data.aes_key_id);

                return {
                    ...data,
                    data: decript_aes(data.data, key)
                }
            })
            .map(async (data_promise) => {
                const data = await data_promise;
                return {
                    sender_id: data.sender_id,
                    reactions: data.reactions,
                    upload_time: data.upload_time,
                    sent_time: data.sent_time,
                    msg_type: data.msg_type,
                    data: data.data,
                } as Message;
            })
        )
        return messages;
    }
}