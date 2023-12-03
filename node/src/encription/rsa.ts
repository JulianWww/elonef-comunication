import { Buffer } from "buffer";
import forge from "node-forge";

/**
 * encrypt some data with rsa.
 * @param toEncrypt the data to encript
 * @param key the node forge key to encrypt with
 * @returns the enrcypted data
 */
function encript_rsa(toEncrypt: Buffer, key: forge.pki.rsa.PublicKey) {
    return Buffer.from(key.encrypt(toEncrypt.toString("base64")))
};

/**
 * decrypt some data with rsa.
 * @param toDecrypt the data to decrypted
 * @param key the node forge key to decrypt with
 * @returns the plain text data
 */
function decript_rsa(toDecrypt: Buffer, key: forge.pki.rsa.PrivateKey) {
    return Buffer.from(key.decrypt(toDecrypt.toString()), "base64")
};

export { encript_rsa, decript_rsa }