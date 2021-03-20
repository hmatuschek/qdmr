fs = require('fs');

fs.readFile('pkts.json', (err, s) => {
    p = JSON.parse(s);
    out = p.filter(function(o) {return  o._source.layers.usb["usb.src"] === "host"});
    let outS = out.map(a => a._source.layers["usb.capdata"])
    fs.writeFile('host-to-dev.txt', outS.join("\n"), (err) => {
        console.log(err);
    });

    inn = p.filter(function(o) {return o._source.layers.usb["usb.src"] != "host"});
    let inS = inn.map(a => a._source.layers["usb.capdata"])
    fs.writeFile('dev-to-host.txt', inS.join("\n"), (err) => {
        console.log(err);
    });
});
