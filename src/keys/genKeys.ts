import crypto from 'crypto';
import { toKeyMap } from '../utility';
import { KeyObject } from "../types";


const format = "pem"
const curve = "sect571r1"
/**
 * Generates equivalent to RSASSA-PKCS1-v1_5 keypair
 */
async function generateKeyPairRSA(): Promise<{
    publicKey: any,
    privateKey: any
  }> {
    return new Promise((resolve, reject) => {
      crypto.generateKeyPair('rsa', {
        modulusLength: 1024,
      }, (err, publicKey, privateKey) => {
        if (err) {
          reject(err);
        } else {
          resolve({
            publicKey: publicKey.export({ format: format, type: "pkcs1" }),
            privateKey: privateKey.export({ format: format, type: "pkcs1" }),
          });
        }
      });
    });
  }

/**
 * build an ECDSA keypair using the sect571r1 curve. These keys will be used to sign and verify the authenticity of messages by the `ConnectionWrapper`
 * @returns a public and a private key in pem format.
 */
async function generateKeyPairECDSA(): Promise<{
  publicKey: string,
  privateKey: string,
}> {
  return new Promise((resolve, reject) => {
    crypto.generateKeyPair('ec', {
      namedCurve: curve,
    }, (err, publicKey, privateKey) => {
      if (err) {
        reject(err);
      } else {
        resolve({
          publicKey: publicKey.export({
            format: format, type: "spki"
          }) as string,
          privateKey: privateKey.export({
            format: format, type: "sec1"
          }) as string
        });
      }
    });
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
        key: pub_data as string,
        signatures: {}
      },
      sign_key: {
        key: publicKey as string,
        signatures: {}
      },
      id
    } as PublicClientKey
  }
}

export { generateKeyPairECDSA, generateKeyPairRSA, format, curve, generateClientKeys }
export type { PublicClientKey, PrivateClientKey, SignedKey }