const { ServerConnectionHandler, generateClientKeys } = require('../lib');

const key = {
  "data_key": {
    "key": "-----BEGIN RSA PUBLIC KEY-----\nMIGJAoGBANwSLpEUDEOzk5rL1VzX0qTpwupNLuysIO3AaxJmyIu1n+ehJrX0N71q\nve9K63tMdq9ahxuXeX3Fcu7KyI9EVx/Gv8arE4JF45Q8HZR3MWDFWq0Ye0M9l+64\n3g1mAokMgrzXrRpYPhEf3/AWdYCdzTh4rPo3+QtbRUufRcT/ZefhAgMBAAE=\n-----END RSA PUBLIC KEY-----\n",
    "signatures": {}
  },
  "sign_key": {
    "key": "-----BEGIN PUBLIC KEY-----\nMIGnMBAGByqGSM49AgEGBSuBBAAnA4GSAAQAwjnU3nsRQUnePJ6UzsO0t8BX9uKy\nLBWZcYI+GEfBA/RjSh3PUMLrFNM6WtHS7grVvgI43YmvMXDlU+FixXaNOInkE72o\nrcgCl6hXKimoTfIuJuCA1HFhAplJSRzRxjzhtaT/G3xzTbNdvoL30okZbst6Jrqh\n+YpQdBFD1EQaUaHzSxagkldcfGB82E1PO9U=\n-----END PUBLIC KEY-----\n",
    "signatures": {}
  }
}

var chat_key = new Map();
var messages = [];
var chat_key_id = ""

const wss = new ServerConnectionHandler(
    { port: 8080 }, 
    id => key,
    (chat_id, key_id) => chat_key.get(key_id),
    (chat_id) => chat_key_id,
    (chat_id, key_id, keys) => {
      chat_key.set(key_id, keys);
      chat_key_id = key_id;
    },
    (chat_id, message) => {
      messages.push(message);
    },
    (chat_id, last_idx, lenght) => messages
  )
  .add_api_callback("test/hi", (data, uid) => {
    console.log(data.toString(), uid);
    return Buffer.from("return data");
  });