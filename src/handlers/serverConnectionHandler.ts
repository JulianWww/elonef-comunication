import { randomBytes } from "crypto";
import { PublicClientKey } from "../keys/genKeys";
import { ConnectionHandler } from "./connectionHandler";
import { WebSocketServer, WebSocket, MessageEvent } from "ws";
import { BufferReader, bufferArrayToBuffer, bufferToBufferArray, bufferToNumber, bufferToString, bufferToStringArray, uuid, uuid_size } from "../encoding";
import { import_public } from "../keys";
import { unsafe_error, verify_nonstreamable } from "../encription/sign";
import { stringify, toKeyMapWithValue, to_byte } from "../utility";
import { nothing } from "../types";
import { Buffer } from "buffer";


/**
 * the sate of connection:
 *  - weather the connection is authenticated or not
 *  - the data used for authentication
 *  - the id of the user corresponding to this connection
 */
class ConnectionState {
    constructor() {
        this.authenticated = false;
        this.auth_data = randomBytes(1024);
    };
    authenticated: boolean;
    auth_data: Buffer
    uid?: string;
}


/**
 * The server side of the connection. Handles data base lookup using callback lambdas and has bindpoints for API calls. It can also perform api calls to every client.
 * It uses callback lumbdas to handle data requests to the database and the server. See constructor for infomation about these callback lambdas.
 */
export class ServerConnectionHandler extends ConnectionHandler {
    private wsServer: WebSocketServer;
    private get_pub_key: (id: string) => Promise<PublicClientKey | nothing>;
    private get_chat_key: (chat_id: string, key_id: string) => Promise<Record<string, string> | nothing>;
    private get_chat_newest_chat_key: (chat_id: string) => Promise<string | nothing>;
    private set_chat_key: (chat_id: string, key_id: string, key: Record<string, string>) => Promise<void>;
    private add_message: (chat_id: string, message: string) => Promise<void | nothing>;
    private get_messages: (chat_id: string, last_idx: number, length: number) => Promise<string[] | nothing>;

    constructor(
            server_options: Record<string, any>,
            get_pub_key: (id: string) => Promise<PublicClientKey | nothing>,
            get_chat_key: (chat_id: string, key_id: string) => Promise<Record<string, string> | nothing>,
            get_chat_newest_chat_key: (chat_id: string) => Promise<string | nothing>,
            set_chat_key: (chat_id: string, key_id: string, key: Record<string, string>) => Promise<void>,
            add_message: (chat_id: string, message: string) => Promise<void>,
            get_messages: (chat_id: string, last_idx: number, length: number) => Promise<string[] | nothing>,
        ) {
        super();

        this.wsServer = new WebSocketServer(server_options);
        this.wsServer.on("connection", this.on_connection);

        this.get_pub_key = get_pub_key;
        this.get_chat_key = get_chat_key;
        this.get_chat_newest_chat_key = get_chat_newest_chat_key;
        this.set_chat_key = set_chat_key;
        this.add_message = add_message;
        this.get_messages = get_messages;
    }

    private async authanticate(ws: WebSocket, con_state: ConnectionState) {
        const data = randomBytes(1024);
        const reader = new BufferReader(
            await this.send(ws, data, ConnectionHandler.AUTH, false)
        );
        const user_id = bufferToString(reader);
        const signature = reader.readRest();
        

        const key = import_public((await this.get_pub_key(user_id))?.sign_key.key) as Buffer | nothing
        if (!key) {
            throw Error("missing public key for user " + user_id);
        }
        if (!verify_nonstreamable(data, signature, key)) {
            throw unsafe_error;
        }
        con_state.authenticated = true;
        con_state.uid = user_id;
    }

    private on_connection = (ws: WebSocket) => {
        const con_state = new ConnectionState();
        ws.addEventListener("message", this.on_message(ws, con_state));
        this.authanticate(ws, con_state);
    }

    protected async handle_single_side_messages(ws: WebSocket, userid: string, message: Buffer, authenticated: boolean, type: number) {
        this.check_auth(ws, authenticated);
        
        if (type == 16) {
            return await this.get_keys_handler(message);
        }
        else if (type == 17) {
            return await this.set_chat_key_handler(message);
        }
        else if (type == 18) {
            return await this.get_newest_chat_key(message, userid);
        }
        else if (type == 19) {
            await this.add_message_handler(message);
        }
        else if (type == 20) {
            return await this.get_signature_keys_handler(message);
        }
        else if (type == 21) {
            return await this.read_messages_handler(message);
        }
        else if (type == 22) {
            return await this.get_chat_key_handler(message, userid);
        }
        return Buffer.from([]);
    }

    private async get_key_handler_subholder(message: Buffer, key_idx: keyof PublicClientKey) {
        return bufferArrayToBuffer(
            ((await Promise.all(
                bufferToStringArray(
                    new BufferReader(message)
                ).map((user: string) => 
                    this.get_pub_key(user)
                )
            ))
            .filter((user: PublicClientKey | nothing) => !! user) as PublicClientKey[])
            .map(
                (key: PublicClientKey) => stringify(key[key_idx])
            )
        )
    }

    private async get_keys_handler(message: Buffer) {
        return this.get_key_handler_subholder(message, "data_key");
    }

    private async get_signature_keys_handler(message: Buffer) {
        return this.get_key_handler_subholder(message, "sign_key");
    }

    private async set_chat_key_handler(message: Buffer) {
        const reader = new BufferReader(message);
        const chat_id = bufferToString(reader);
        const key = toKeyMapWithValue(
            bufferToBufferArray(
                reader
            ).map((buf: Buffer) => {
                const reader = new BufferReader(buf);
                return {
                    name: bufferToString(reader),
                    key: reader.readRest().toString("base64")
                }
            }
            ), 
            "name",
            "key"
        ) as Record<string, string>;
        const key_id = uuid().toString("base64");

        this.set_chat_key(chat_id, key_id, key)
        return to_byte(1);
    }

    private async get_chat_key_handler(message: Buffer, userid: string) {
        const reader = new BufferReader(message);
        const key_id = reader.read(uuid_size).toString("base64");
        const chat_id  = reader.readRest().toString();
        const key = (await this.get_chat_key(chat_id, key_id));

        if (!key) {
            return Buffer.from("")
        }
        return Buffer.from(key[userid], "base64");
    }

    private async get_newest_chat_key(message: Buffer, userid: string) {
        const reader = new BufferReader(message);
        const chat_id = bufferToString(reader);
        const key_id = await this.get_chat_newest_chat_key(chat_id);
        if (!key_id) {
            console.log("failed to get chat key id for chat " + chat_id);
            return Buffer.from("");
        }
        const key = (await this.get_chat_key(chat_id, key_id));
        if (!key) {
            console.log("failed to get chat key for chat " + chat_id);
            return Buffer.from("");
        }

        return Buffer.concat([
            Buffer.from(key_id, "base64"),
            Buffer.from(key[userid], "base64"),
        ])
    }

    private async add_message_handler(message: Buffer) {
        const reader = new BufferReader(message);
        const chat_id = bufferToString(reader);
        const content = reader.readRest().toString("base64");
        this.add_message(chat_id, content);
    }

    private async read_messages_handler(message: Buffer) {
        const reader = new BufferReader(message);
        const chat_id = bufferToString(reader);
        const last_idx = bufferToNumber(reader);
        const length = bufferToNumber(reader);

        var messages = await this.get_messages(chat_id, last_idx, length);
        if (!messages) {
            messages = [];
        }
        return bufferArrayToBuffer(messages.map((vale: string) => Buffer.from(vale, "base64")));
    }

    private on_message = (ws: WebSocket, con_state: ConnectionState) => {

        return (message: MessageEvent) => {
            this.handle_message(ws, con_state.uid ? con_state.uid : "", message.data as Buffer, con_state.authenticated);
        }
    }
}