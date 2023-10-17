const { ServerConnectionHandler, ClientConnectionHandler, generateClientKeys } = require('../lib');

const client_key = {
  sign_key: 'KMe70BSIf4rSl6xJ85yKs01YLPrL0ipRT+GKwzCORKY=',
  data_key: '-----BEGIN RSA PRIVATE KEY-----\r\n' +
    'MIIEpQIBAAKCAQEA5zCHHTLAnOqKv20X374YZtWj3c9HAE26dIcYZiQ6RwFReZkk\r\n' +
    'sIg6ReYx0IAZR/QZg+Uy8Ul0S+44SqpYd0zV/UFZX/Rr6F09+rm8JjzVMOmxAzWl\r\n' +
    'MpfZkyUBc9239nHy5aWZOm8HzQ8AyAmQq3llxMp1Q8esITZ9LHx3285cj1q51fsV\r\n' +
    'Pyq6oQOSpVGyNnjoRLetavPNN6PA1HF18V+pEDcwMoHygKRgdVP0oBboA/uMKjmi\r\n' +
    'kK3r6jbXGm1OqYRdyiF1X7j8rPMmMhPB0NttQwOeNg99AAOQpQGWzuyMyLd5x8u7\r\n' +
    'si4brC5zlDT9nRO5RZjC2RuBkbWhoVWHhHrUVwIDAQABAoIBAAC3S2z5An0BCHGD\r\n' +
    'GweNjdh9t8f/FFOUHt+RCfSruZuPKiMA/G5PNtc9wMZ45fKCxG9d9n66bVXDHr5+\r\n' +
    'nNlF7aFBftuB9z+3F0pqBQDqZG0swCtzLzmuNGU8H93mdRQkfRcPLLoKqmalduI3\r\n' +
    '+Agc34R/B+UFcpLZmasy2xq2jHiVPo4ligXKvL9ML9/M25Ug6kywuEa3V9gAgaUh\r\n' +
    '5ivpWJ6s9GIzT3xorscFKNwACVKpPUgP7ZYkHsZRUcWX4eMFdV9Yh7i0jUuQ6FoC\r\n' +
    'XyFkoQ/ObXQqd/LRQQUlAEi5sFp+QxxnbpYEwMWZ1ef5qdvKJgRwaqHlFitL/2Sx\r\n' +
    'MG5wBIECgYEA6+O+bLkEnqoG9YPoAu9yau7nk/BmHTjpFc4mO3+i2mUzAST5jvgD\r\n' +
    'Pszb921zGsP/gAitzWUpPtSnOuvrC8QW6o/1MYBUHsE/A6EUAinjLZ9ecwxSRhCL\r\n' +
    'JwdWDfOm35ia4V/HqxUak5KQSodH4I9JOJ6uB7kObSQouDugVuoNdBcCgYEA+uY0\r\n' +
    'rUqrw/3RCHSilIcLDgToAYZntBB3SMzFEa53hwcgRxoXSgBWpBrdPmH6rtcW7VHZ\r\n' +
    '6iTmroizCIWNjECGZjh5UYzD+59KXxKUUtF0rpArPKxPQj8i2bUlrbf4azEEVFZ5\r\n' +
    'VigR/n6OEKoCC04MIeU2qG8Qvi1a6Um+0vA6icECgYEAk6xr1hyt67Z9zM3bPZsx\r\n' +
    'ggg2KcNh6gFqyh/Yh38MMLUSDTFcmkhhn7sYKfn3CVaSaVEFnYQrIg528sgmyaLk\r\n' +
    'xcuBsI+qGebY0CjTIdefRTx0IIoiJ4LL2o8ZLHrQiUI8E8OmeFAp5yDl/AJc2RRa\r\n' +
    'vnt4b5VGlsUASPXrFv5HldkCgYEAu8QIbB/TyAttolxXR7yAo288ideiSHmCK7ND\r\n' +
    'UGY3AZBtsYhvIWx7arjIFmWRC8PgXp7A7J/NG5UmFxyLwAiSDHH7KPKlwOuG5vuq\r\n' +
    'kQg7wGAYW4HAlx1U4YTVu9pQSTgXu14z1CyMut7PRt1URk2UetFlRExKRWj//lK4\r\n' +
    'QniR9oECgYEAm4rarX3jwARh5jAjlF67j/cwSTsJL7qzCf1tzTsZjZm1kZq7pOxx\r\n' +
    'BmCPqpgGqBa56vCtbKuaQ7F5sJNlcUIK3TceIRHLr12TYBQATmW0k5DAntplZprH\r\n' +
    'MnZOGgFzJ08v8IkmNsYe/CtywsLKvwhrlS8UiJbjqF+5iMrioevxyZg=\r\n' +
    '-----END RSA PRIVATE KEY-----\r\n',
  server_key: 'SERVER_KEY',
  ca_keys: {},
  uid: 'd'
}


const key = {
  data_key: {
    key: '-----BEGIN PUBLIC KEY-----\r\n' +
      'MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA5zCHHTLAnOqKv20X374Y\r\n' +
      'ZtWj3c9HAE26dIcYZiQ6RwFReZkksIg6ReYx0IAZR/QZg+Uy8Ul0S+44SqpYd0zV\r\n' +
      '/UFZX/Rr6F09+rm8JjzVMOmxAzWlMpfZkyUBc9239nHy5aWZOm8HzQ8AyAmQq3ll\r\n' +
      'xMp1Q8esITZ9LHx3285cj1q51fsVPyq6oQOSpVGyNnjoRLetavPNN6PA1HF18V+p\r\n' +
      'EDcwMoHygKRgdVP0oBboA/uMKjmikK3r6jbXGm1OqYRdyiF1X7j8rPMmMhPB0Ntt\r\n' +
      'QwOeNg99AAOQpQGWzuyMyLd5x8u7si4brC5zlDT9nRO5RZjC2RuBkbWhoVWHhHrU\r\n' +
      'VwIDAQAB\r\n' +
      '-----END PUBLIC KEY-----\r\n',
    signatures: {}
  },
  sign_key: {
    key: 'BKXrSlEgv6tIUDyCLWZuEa4CqxGTNEY/hZft3mXGBZQc6xqJLi8SffGhMoxKNIO8csO7QbJ2e0otBN4pO5WCPXc=',
    signatures: {}
  },
  id: 'd'
}

//generateClientKeys("SERVER_KEY", [], "denanu").then(console.log)

class Pipe {
  msg_handler
  other
  name
  constructor(name) {
    this.name = name
  }

  addEventListener(type, handler) {
    if (type === "message") {
      this.msg_handler = handler
    }
  }

  send(data) {
    //console.log(this.name, data)
    this.other.msg_handler({data})
  }

  close() {
    console.log("connection closed, (not realy in the dummy)")
  }
}

const s2c = new Pipe("s => c");
const c2s = new Pipe("c => s");
s2c.other = c2s
c2s.other = s2c

var chat_key = new Map();
var messages = [];
var chat_key_id = ""


const client = new ClientConnectionHandler(c2s, client_key)
client.add_api_callback("client/api/callback", (data, uid) => {
  console.log(data, uid);
  return Buffer.from("")
})

const wss = new ServerConnectionHandler(
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
    console.log("hi")
    wss.make_api_request("client/api/callback", ["d", "other"], Buffer.from("hello"))
    return Buffer.from("return data");
  });

wss.on_connection(s2c)

client.generate_chat_keys(["denanu", "d"], "test_chat")
.then(v =>
  client.send_message("hello this is me :)", 0, "test_chat")
)
.then(v => 
  client.make_api_request("test/hi", Buffer.from("hi"))
)
.then(v=> 
  client.get_message("test_chat", 0, 100)
)
.then(v=> {
  console.log("key_after gen")
  console.log(v.map(msg => {return {...msg, data: msg.data.toString()}}))
})