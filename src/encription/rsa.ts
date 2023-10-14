import { Buffer } from "buffer";
import forge from "node-forge";


function encript_rsa(toEncrypt: Buffer, key: forge.pki.rsa.PublicKey) {
    return Buffer.from(key.encrypt(toEncrypt.toString("base64")))
};

function decript_rsa(toDecrypt: Buffer, key: forge.pki.rsa.PrivateKey) {
    return Buffer.from(key.decrypt(toDecrypt.toString()), "base64")
};

export { encript_rsa, decript_rsa }