const { encript_aes, decript_aes, create_aes_key } = require("../lib/index.js");

function test_aes(txt) {
    key = create_aes_key()
    enc = encript_aes(txt, key)
    dec = decript_aes(enc, key)
    return false
}

test(
    "test_en_and_decryptions", () => {
        expect(()=>test_aes("the moon came falling down onto the boor unsuspecting wm15.")).toBe(true);
    }
)