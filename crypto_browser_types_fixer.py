file = """
/// <reference types="node" />
/// <reference types="node" />
import { JsonWebKeyInput, KeyObject, PrivateKeyInput, PublicKeyInput } from "crypto";
import { nothing } from "../types";
import { Buffer } from "buffer";
declare function import_private(key_data: string | PrivateKeyInput | Buffer | JsonWebKeyInput | KeyObject): KeyObject;
declare function import_public(key_data: string | KeyObject | Buffer | JsonWebKeyInput | PublicKeyInput | KeyObject | nothing): KeyObject | null;
export { import_private, import_public };
"""

from subprocess import run
from re import compile
crypto_patern = 'import { ([A-Za-z, ]+) } from "crypto";'
searcher = compile(crypto_patern)


def search(data: str):
    lines = data.split("\n")
    res = []
    for line in lines:
        out = line
        reg_found = list(searcher.findall(line))
        if (len(reg_found)):
            out = "\n".join([f"type {object} = any" for object in reg_found[0].split(",")])
        
        res.append(out)
    return "\n".join(res)

print(run(["find lib | grep \.d\.ts | grep -v node_modules"], shell=True, capture_output=True, text=True).stdout)

print(search(file))
