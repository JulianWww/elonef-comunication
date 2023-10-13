import NodeRSA from "node-rsa";
import { Buffer } from "buffer";
import forge from "node-forge";


function encript_rsa(toEncrypt: Buffer, key: NodeRSA) {
    return key.encrypt(toEncrypt)
};

function decript_rsa(toDecrypt: Buffer, key: NodeRSA) {
    return key.decrypt(toDecrypt)
};

export { encript_rsa, decript_rsa }