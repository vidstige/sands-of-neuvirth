require("../style.css");

var n;
var x = [];
var y = [];
var z = [];

function draw(timestamp) {
    const canvas = document.getElementById('canvas');
    const ctx = canvas.getContext('2d');
    ctx.fillStyle = getComputedStyle(canvas).color;

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    const s = 10;
    for (var i = 0; i < n; i++) {
        const xx = x[i];
        const yy = y[i];
        //console.log(xx, yy);
        ctx.fillRect(
            canvas.width / 2 + xx * s,
            canvas.height / 2 + yy * s,
            3, 3);
    }
}

const consume = responseReader => {
    return responseReader.read().then(result => {
        if (result.done) {  return; }
        console.log('frame');
        // do something with the current chunk
        n = result.value.length / 12;
        if (n != 100) {
            console.warn(n);
        }
        var xx = new DataView(result.value.buffer, n * 4 * 0, n * 4);
        var yy = new DataView(result.value.buffer, n * 4 * 1, n * 4);
        var zz = new DataView(result.value.buffer, n * 4 * 2, n * 4);

        //console.log("OK!");
        for (var i = 0; i < n; i++) {
            x[i] = xx.getFloat32(i*4, true);
            y[i] = yy.getFloat32(i*4, true);
            z[i] = zz.getFloat32(i*4, true);
        }
        
        requestAnimationFrame(draw);

        return consume(responseReader);
    });
}

function load() {
    const url = '/api/sand';
    fetch(url).then(response => {
        return consume(response.body.getReader());
    })
    .catch(console.error);
}

document.addEventListener("DOMContentLoaded", load);
