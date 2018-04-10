require("../style.css");

const consume = responseReader => {
    return responseReader.read().then(result => {
        if (result.done) { return; }

        // do something with the current chunk
        console.log(result.value.length);
        var chunk = new DataView(result.value.buffer);
        console.log(chunk.getFloat32(0, true));

        return consume(responseReader);
    });
}

// Perform the request and consume response stream

function load() {
    const url = '/api/sand';
    fetch(url).then(response => {
        return consume(response.body.getReader());
    })
    .catch(console.error);
}

document.addEventListener("DOMContentLoaded", load);

