const { encript_aes, decript_aes, create_aes_key } = require("../lib/index.js");

function test_aes(txt) {
    key = create_aes_key()
    enc = encript_aes(txt)
    dec = decript_aes(enc)
    return txt === dec
}

test(
    "test_en_and_decryptions", () => {
        expect(()=>test_aes("the moon came falling down onto the boor unsuspecting wm15.")).toBeTruthy();
    }
)