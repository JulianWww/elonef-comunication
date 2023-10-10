import { privateDecrypt, publicEncrypt } from "crypto";
import { Buffer } from "buffer";
import { KeyObject } from "../types";

function encript_rsa(toEncrypt: Buffer, key: KeyObject) {
    return publicEncrypt(key, toEncrypt);
};

function decript_rsa(toDecrypt: Buffer, key: KeyObject) {
    return privateDecrypt(key, toDecrypt);
};

export { encript_rsa, decript_rsa }