const { write, writeFileSync } = require('fs');
var net = require('net');

var host = '127.0.0.1';
var port = 4000;

var socket = new net.Socket();

socket.connect(port, host, () => {

    const json = {
        command: "start",
        protocol: "MOESI"
    }

    socket.write(JSON.stringify(json) + "\n");
});

socket.on('data', (data) => {

    const json = JSON.parse(data);
    const trans = json.transactions;
 

    for(const t of trans){
        console.log(t);

    }
    writeFileSync("report.json", JSON.stringify(json));
    socket.destroy();
});
