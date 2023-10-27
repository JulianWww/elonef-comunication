import uuid, { uuid_size } from "./uuid";
import BufferReader from "./bufferReader";

/**
 * Converts an intager > 0 and < 2^(2^4096) to a buffer
 * @param num {number} - the number to be conveted to buffer
 */
function numberToBuffer(num: number) {
    const out = Array();
    while (num > 0) {
        out.push(num % 256)
        num = num >> 8
    }
    return Buffer.from([
        out.length,
        ...out
    ])
}

/**
 * converts a number to buffer. The number must be 0 <= num < 256. The number is stored in a single byte
 * @param num {number} - the number to be converted to a buffer
 */
function shortNumberToBuffer(num: number) {
    return Buffer.from([num % 256]);
}

/**
 * converts a buffet to a number. using a single byte of data.
 */ 
function bufferToShortNumber(buff: BufferReader) {
    return buff.read(1)[0]
}


/**
 * reads an intager out of a Buffer wrapped by a BufferReader
 * @param stream {BufferReader} - the stream to read the data from
 */
function bufferToNumber(stream: BufferReader) {
    const len = stream.read(1)[0];
    var value = 0;
    for (var idx=0; idx < len; idx++) {
        value = value + (stream.read(1)[0] << (8 * idx));
    }
    return value;
}

/**
 * converts a string (or Buffer with lenght not known at build time) to a buffer
 * @param str {string | Buffer} - the string to be converted to Buffer, can also be a dynamic lenth buffer
 * @param number_encoder {(num: number) => Buffer} - the encoding function used to encode the length of the string
 */
function stringToBuffer(str: string | Buffer, number_encoder: (num: number) => Buffer=numberToBuffer) {
    return Buffer.concat([
        number_encoder(str.length),
        Buffer.from(str)
    ])
}

/**
 * extracts a dynamic size buffer from from the Buffer
 * @param stream {BufferReader} - the sream provideing buffer content
 * @param number_decoder {(stream: BufferReader) => number} - the decoder function used extract the size of the Buffer
 */
function extractDynamicBuffer(stream: BufferReader, number_decoder: (stream: BufferReader) => number=bufferToNumber) {
    const len = number_decoder(stream);
    return stream.read(len);
}

/**
 * extracts a string form the bufferstream
 * @param stream {BufferReader} - the stream prviding buffer content
 * @param number_decoder {(stream: BufferReader) => number} - the decoder function used extract the size of the string
 */
function bufferToString(stream: BufferReader, number_decoder: (stream: BufferReader) => number=bufferToNumber) {
    const str = extractDynamicBuffer(stream, number_decoder).toString();
    return str;
}

/**
 * write a buffer aray to a buffer.
 * @param buffers the buffers that should be write to the buffer
 * @returns the encoded buffer
 */
export function bufferArrayToBuffer(buffers: (string | Buffer)[]) {
    return Buffer.concat(
        [
            numberToBuffer(buffers.length),
            ...buffers.map(
                (buf: Buffer | string) => stringToBuffer(buf)
            )
        ]
    )
}

export function bufferToBufferArray(reader: BufferReader) {
    const length = bufferToNumber(reader);
    const out = []
    for (var idx=0; idx<length; idx++) {
        out.push(extractDynamicBuffer(reader));
    }
    return out;
}

export function bufferToStringArray(reader: BufferReader) {
    return bufferToBufferArray(reader).map((value: Buffer) => value.toString());
}

/**
 * writes a messag to buffer and ecodes it with base64
 * @param msg {Buffer} - the message encripted with AES
 * @param decriptionData {Record<string, Buffer>} - The decription keys encripted with an asymetric encription
 */
/*function messageToBuffer(msg: Buffer, decriptionData: Record<string, Buffer>) {
    const out = Array();

    // add the buffer
    out.push(stringToBuffer(msg))
    out.push(
        ...Object.entries(decriptionData).map(
            ([key, data]: [string, Buffer]) => {
                return Buffer.concat([
                    stringToBuffer(key),
                    data
                ])
            }
        )
    )
    return Buffer.concat(out)
}
*/
/**
 * converts a buffer to a Message with decription information
 * @param buff {Buffer} - the buffer containing all the information
 * @param keySize {number} - the size of the decription key
 */
/*function bufferToMessage(buff: Buffer, keySize: number) {
    const stream = new BufferReader(buff);
    const msg = extractDynamicBuffer(stream);
    
    const decypherData = new Map<string, Buffer>();
    while (stream.canRead()) {
        decypherData.set(
            bufferToString(stream),
            stream.read(keySize)
        )
    }
    return [msg, decypherData] as [Buffer, Map<string, Buffer>]
}*/

export { 
    //messageToBuffer, 
    //bufferToMessage, 
    uuid, 
    uuid_size, 
    numberToBuffer, 
    bufferToNumber, 
    stringToBuffer, 
    bufferToString,
    extractDynamicBuffer, 
    bufferToShortNumber, 
    shortNumberToBuffer, 
    BufferReader
}