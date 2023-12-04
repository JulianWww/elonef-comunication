import { generatePrivate as generatePrivateECDSA, getPublic as getPublicECDSA } from "eccrypto";
import { toKeyMap } from '../utility';
import forge from "node-forge";


const format = "pem"
const curve = "secp256k1"
/**
 * Generates equivalent to RSASSA-PKCS1-v1_5 keypair
 */
function generateKeyPairRSA() {
   
      var keypair = forge.pki.rsa.generateKeyPair(2048, 0x10001);
      return ({
        privateKey: forge.pki.privateKeyToPem(keypair.privateKey),
        publicKey:  forge.pki.publicKeyToPem(keypair.publicKey)
      })
  }

/**
 * build an ECDSA keypair using the sect571r1 curve. These keys will be used to sign and verify the authenticity of messages by the `ConnectionWrapper`
 * @returns a public and a private key.
 */
function generateKeyPairECDSA(): Promise<{
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

/**
 * a key signed by any number of CAs. This is used for RSA and ECDSA by the PublicClientKey
 */
interface SignedKey {
  /**
   * The data containing the actual Key
   */
  key: string,
  /**
   * the signatures of this key. (currently unused)
   */
  signatures: Record<string, string>
}

/**
 * A clients public key. Used to authenticate that client, sign messages and communicate the chat keys. (the aes keys used to encript messages in a certain chat)
 */
interface PublicClientKey {
  /**
   * the Clients Public RSA key
   */
  data_key: SignedKey,
  /**
   * the clients Public ECDSA key
   */
  sign_key: SignedKey,
  /**
   * the user id of the client
   */
  id: string
}

/**
 * A clients private key used to create a connection to the server and handle the client side 
 */
interface PrivateClientKey {
  /**
   * the private ECDSA key
   */
  sign_key: string,
  /**
   * the privat RSA Key
   */
  data_key: string,
  /**
   * the ECDSA key of the server (currently unused)
   */
  server_key: string,
  /**
   * the public keys of the CA's (currntly unused)
   */
  ca_keys: Record<string, string>,
  /**
   * the user id of the user.
   */
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

export { generateClientKeys }
export type { PublicClientKey, PrivateClientKey, SignedKey }