import NodeRSA from "node-rsa";
import { generatePrivate as generatePrivateECDSA, getPublic as getPublicECDSA } from "eccrypto";
import { toKeyMap } from '../utility';
import { KeyObject } from "../types";
import forge from "node-forge";


const format = "pem"
const curve = "secp256k1"
/**
 * Generates equivalent to RSASSA-PKCS1-v1_5 keypair
 */
function generateKeyPairRSA() {
   
      const rsa = new NodeRSA({b: 2048});
      var keypair = forge.pki.rsa.generateKeyPair(2048, 0x10001);
      
      return ({
        privateKey: rsa.exportKey("pkcs1"),
        publicKey: rsa.exportKey("pkcs1-public")
      })
  }

/**
 * build an ECDSA keypair using the sect571r1 curve. These keys will be used to sign and verify the authenticity of messages by the `ConnectionWrapper`
 * @returns a public and a private key.
 */
async function generateKeyPairECDSA(): Promise<{
  publicKey: string,
  privateKey: string,
}> {
  return new Promise((resolve, reject) => {
    const priv = generatePrivateECDSA();
    const pub = getPublicECDSA(priv);

    resolve({
      privateKey: priv.toString("base64"),
      publicKey: pub.toString("base64")
    })
  });
}

interface SignedKey {
  key: string,
  dec_key: KeyObject,
  signatures: Record<string, string>
}

interface PublicClientKey {
  data_key: SignedKey,
  sign_key: SignedKey,
  id: string
}

interface PrivateClientKey {
  sign_key: string,
  data_key: string,
  server_key: string,
  ca_keys: Record<string, string>,
  uid: string,
}

/**
 * generates the private and public keys for a certain client. The private key should be stored for later use, while the public key should be put ont the servers database and signed with the CA keys.
 * @param server_key the public key of the server. used to verify the authenticity of messages
 * @param ca_keys the public keys of the CA's used to verify the authenticity of keys recieved fromt the server.
 * @param id the users it.
 * @returns the public and private keys of the client.
 */
async function generateClientKeys(server_key: string, ca_keys: any[], id: string) {
  const { privateKey, publicKey } = await generateKeyPairECDSA();
  const { privateKey: priv_data, publicKey: pub_data } = await generateKeyPairRSA();
  return {
    privateKey: {
      sign_key: privateKey,
      data_key: priv_data,
      server_key,
      ca_keys: toKeyMap(ca_keys, "id"),
      uid: id,
    } as PrivateClientKey,
    publicKey: {
      data_key: {
        key: pub_data,
        signatures: {}
      },
      sign_key: {
        key: publicKey,
        signatures: {}
      },
      id
    } as PublicClientKey
  }
}

export { generateKeyPairECDSA, generateKeyPairRSA, format, curve, generateClientKeys }
export type { PublicClientKey, PrivateClientKey, SignedKey }