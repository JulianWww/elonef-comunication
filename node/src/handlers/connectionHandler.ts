import { WebSocket } from "ws";
import { BufferReader, bufferToString, stringToBuffer, uuid } from "../encoding";
import { to_byte } from "../utility";
import { Buffer } from "buffer";
import { ForwardedError, RemoteError } from "../erros";

export class ConnectionHandler {
    request_resolvers: Map<string, (data: Buffer, ws: WebSocket, authenticated: boolean, success: number) => void>;

    api_callbacks: Map<string, {
        callback: (data: Buffer, uid: string) => Promise<Buffer>,
        auth_needed: boolean
    }>;

    error_handler: (error: string) => void;

    static RETURN = 0;
    static API = 1;
    static AUTH = 2;


    constructor(error_handler: (err: string) => void) {
        this.error_handler = error_handler;
        this.request_resolvers = new Map<string, (data: Buffer) => void>();
        this.api_callbacks = new Map();
    }

    add_api_callback(name: string, callback: (data: Buffer, uid: string) => Promise<Buffer>, auth_needed: boolean=true) {
        this.api_callbacks.set(name, {callback, auth_needed: auth_needed});
        return this;
    }

    protected send(ws: WebSocket, message: Buffer | string, type: number, kill_on_unauth: boolean = true, hold:boolean=true): Promise<Buffer> {
        const uid = uuid();
        
        const promise = new Promise<Buffer>((resolve, reject) => {
            this.request_resolvers.set(
                uid.toString("base64"), 
                (data: Buffer, ws: WebSocket, authenticated: boolean, success: number) => {
                    if (!success) {
                        const fail_reason = data.toString();
                        console.log(fail_reason)

                        this.error_handler(fail_reason)
                        reject(new RemoteError(fail_reason))
                        
                        return;
                    }
                    if (kill_on_unauth) {
                        if (this.check_auth(ws, authenticated)) {
                            reject("auth failed");
                            return;
                        }
                    }
                    resolve(data);
                }
            );
        });
        ws.send(Buffer.concat([uid, Buffer.from([type]), Buffer.from(message)]));

        return promise;
    }

    protected recv(ws: WebSocket, uuid: Buffer, data: Buffer, authenticated: boolean){
        const uid = uuid.toString("base64");
        const success = data[0];
        const msg = data.subarray(1);

        
        this.request_resolvers.get(uid)?.(msg, ws, authenticated, success);
        this.request_resolvers.delete(uid);
    }

    protected async handle_message(ws: WebSocket, userid: string, message: Buffer, authenticated: boolean) {
        const uid = message.subarray(0, 16);
        const type = message[16];
        const content = message.subarray(17);

        console.log("got message", type, uid)
        try {

            var out = Buffer.from([]);
            if (type == ConnectionHandler.RETURN) {
                this.recv(ws, uid, content, authenticated);
                return;
            }
            else if (type == ConnectionHandler.API) {
                out = await this.handle_api_call(ws, userid, content, authenticated);
            }
            else if (type == ConnectionHandler.AUTH) {
                out = await this.handle_auth(ws, userid, content, authenticated);
            }
            else {
                const returned = await this.handle_single_side_messages(ws, userid, content, authenticated, type);
                if (!returned) {
                    return;
                }
                out = returned
            }

            ws.send(Buffer.concat([uid, to_byte(ConnectionHandler.RETURN), to_byte(1), out]))
        }
        catch (err) {
            if ((err as Error).name === "ForwardedError") {
                ws.send(Buffer.concat([
                    uid, 
                    to_byte(ConnectionHandler.RETURN), 
                    to_byte(0), 
                    Buffer.from((err as Error).message)
                ]));
            }
            else {
                throw err;
            }
        }
    }

    protected async handle_single_side_messages(ws: WebSocket, userid: string, message: Buffer, authenticated: boolean, type: number): Promise<Buffer | undefined> {
        return undefined
    }

    protected async handle_auth(ws: WebSocket, userid: string, message: Buffer, authenticated: boolean) {
        return Buffer.from([]);
    }

    protected handle_api_call(ws: WebSocket, uid: string, message: Buffer, authenticated: boolean) {
        const reader = new BufferReader(message);
        const call_id = bufferToString(reader);
        const data = reader.readRest();

        const handler = this.api_callbacks.get(call_id);
        if (!handler) {
            throw new Error("Unknown api call: " + call_id);
        }
        if (handler.auth_needed) {
            this.check_auth(ws, authenticated);
        }
        return handler.callback(data, uid);
    }

    async _make_api_request(ws: WebSocket, request_id: Buffer | string, data: Buffer | string, waitForAuth:boolean=true) {
        //await this.ready;
        return this.send(ws, Buffer.concat([
            stringToBuffer(request_id),
            Buffer.from(data),
        ]), ConnectionHandler.API, true, waitForAuth);
    }

    protected check_auth(ws: WebSocket, authenticated: boolean) {
        if (!authenticated) {
            ws.close();
            console.log("Unauthorized connection");
            return true;
        }
        return false;
    }
}