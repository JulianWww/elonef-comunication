import { Buffer } from "buffer";

export function toKeyMap <T> (arr: T[], key: keyof T) {
    const out = new Map();
    arr.forEach(element => {
        const element_cpy = {...element};
        delete element_cpy[key];
        out.set(element[key], element_cpy);
    });
    return Object.fromEntries(out);
}

export function toKeyMapWithValue <T> (arr: T[], key: keyof T, value: keyof T) {
    const out = new Map();
    arr.forEach(element => {
        out.set(element[key], element[value]);
    });
    return Object.fromEntries(out);
}

export function to_byte(value: number) {
    return Buffer.from([value]);
}


export function stringify(value: any) {
    return JSON.stringify(value, null, "\t");
}