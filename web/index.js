let images = [];

function setU16(into, index, val) {
    const buf = new ArrayBuffer(2);
    const view16 = new Uint16Array(buf);
    view16[0] = val;

    const view8 = new Uint8Array(buf);
    into[0] = view8[0];
    into[1] = view8[1];
}

function bitImage(w, h, pix) {
    return { w: w, h: h, p: pix };
}

function bitAt(bi, x, y) {
    return bi.p[y*bi.w + x];
}

function dither(l) {
    switch(l) {
        case 0: return bitImage(4, 4, [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
        case 1: return bitImage(4, 4, [0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1]);
        case 2: return bitImage(4, 4, [0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0]);
        case 3: return bitImage(4, 4, [0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0]);
        case 4: return bitImage(4, 4, [1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1]);
        case 5: return bitImage(4, 4, [1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0]);
        case 6: return bitImage(4, 4, [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]);
    }
}

function drawBitImage(bi, ctx) {
    const d = ctx.createImageData(bi.w, bi.h);

    for(let i = 0; i < bi.w * bi.h; i++) {
        const ii = i*4;
        if (bi.p[i] === 1) {
            d.data[ii] = 0; d.data[ii+1] = 0; d.data[ii+2] = 0; d.data[ii+3] = 255;
        } else {
            d.data[ii] = 255; d.data[ii+1] = 255; d.data[ii+2] = 255; d.data[ii+3] = 255;
        }
    }

    ctx.putImageData(d, 0, 0);
}

function bitImgLightness(bi) {
    let sum = 0;

    for(let i = 0; i < bi.w * bi.h; i++) {
        sum += bi.p[i];
    }

    return 1 - sum/(bi.w * bi.h);
}

function gcLevel(r, g, b, a) {
    return a*(0.299 * r + 0.587*g + 0.114*b);
}

function bitImgGCLimits(d) {
    let max = 0;
    let min = 1;

    for(let i = 0; i < d.width*d.height; i++) {
        const gc = gcLevel(d.data[i]/255, d.data[i+1]/255, d.data[i+2]/255, d.data[i+3]/255);
        if (gc > max) { max = gc; }
        if (gc < min) { min = gc; }
    }

    return [min, max];
}

function toBitImg(c, w, h) {
    const d = c.getImageData(0, 0, w, h);

    const p = new Array(w*h);

    let dithers = [];
    let lightnesses = [];

    for(let i =0 ; i < 7; i++) {
        dithers.push(dither(i));
        lightnesses.push(bitImgLightness(dithers[i]));
    }

    const [minGC, maxGC] = bitImgGCLimits(d);

    for(let x = 0; x < d.width; x++) {
        for(let y = 0; y < d.height; y++) {
            const i = y*d.width*4 + x*4;
            const pi = y*d.width + x;
            let gc = gcLevel(d.data[i]/255, d.data[i+1]/255, d.data[i+2]/255, d.data[i+3]/255);
            gc = (gc - minGC) / (maxGC - minGC);
            let minIdx = 0;
            let minErr = Math.abs(gc - lightnesses[0]);

            for(let di = 1; di < dithers.length; di++) {
                const curErr = Math.abs(gc - lightnesses[di]);
                if (curErr < minErr) {
                    minIdx = di;
                    minErr = curErr;
                }
            }

            p[pi] = bitAt(dithers[minIdx], x%4, y%4);
        }
    }

    return bitImage(w, h, p);
}

function parse(str) {
    const parts = [];
    let curPart = "";
    const state = { bold: false, inv: false, img: null, escaped: false };
    let idx = 0;

    function stateChanged() {
        parts.push({ type: 't', txt: curPart, bold: state.bold, inv: state.inv });
        curPart = "";
    }

    while(idx < str.length) {
        const ch = str.charAt(idx);

        switch(ch) {
            case '\\':
                if (state.escaped) { curPart += '\\'; state.escaped = false; }
                else { state.escaped = true }
                break;
            case '*':
                if (state.escaped) { curPart += '*'; state.escaped = false; }
                else { stateChanged(); state.bold = !state.bold; }
                break;
            case '^':
                if (state.escaped) { curPart += '^'; state.escaped = false; }
                else { stateChanged(); state.inv = !state.inv; }
                break;
            case '@': {
                stateChanged();
                idx++;
                let ich = str.charAt(idx);
                let numStr = "";
                while (idx < str.length && ich >= '0' && ich <= '9') {
                    numStr += ich;
                    idx++;
                    ich = str.charAt(idx);
                }

                const ii = parseInt(numStr);
                parts.push({ type: 'i', i: ii });
                break;
            }
            default:
                curPart += ch;
                state.escaped = false;
                break;
        }

        idx++;
    }

    if (curPart.length > 0) {
        stateChanged();
    }

    return parts.filter(p => p.type == 'i' || p.txt.length > 0);
}

function styleByte(part) {
    return (part.bold ? 1 : 0) << 1 | (part.inv ? 1 : 0) << 2;
}

function preview(parts) {
    const el = document.querySelector("#preview");
    el.innerHTML = "";

    parts.forEach(elem => {
        if (elem.type == 't') {
            const span = document.createElement("span");
            span.appendChild(document.createTextNode(elem.txt));
            if (elem.inv) { span.classList.add("inv"); }
            if (elem.bold) { span.classList.add("bold"); }
            el.appendChild(span);
        } else if (elem.type == 'i') {
            if (elem.i < images.length) {
                const cv = document.createElement("canvas");
                cv.width = images[elem.i].w;
                cv.height = images[elem.i].h;
                const ctx = cv.getContext('2d');
                drawBitImage(images[elem.i], ctx);
                el.appendChild(cv);
            }
        }

    });
}


function strToBytes(str) {
    const enc = new TextEncoder();
    return enc.encode(str);
}

function sendCommand(cmd, data) {
    let xhr = new XMLHttpRequest();
    xhr.open("POST", "/print", true);

    const buf = new ArrayBuffer(3+data.length);
    const view = new Uint8Array(buf);
    setU16(view, 0, 3+data.length);

    view[2] = cmd;

    for(let i = 0; i < data.length; i++) {
        view[3+i] = data[i];
    }

    xhr.send(buf);

    return new Promise(resolve => {
        xhr.onreadystatechange = () => {
            if (xhr.readyState === 4) { resolve(); }
        }
    });
}

function power() {
    let xhr = new XMLHttpRequest();
    xhr.open("POST", "/power", true);
    xhr.send(null);
}

function printText(txt) {
    sendCommand(0, strToBytes(txt));
}

document.querySelector("#input").addEventListener("keyup", e => {
    preview(parse(e.target.value));
});

document.querySelector("#image-upload").addEventListener("input", e => {
    const parts = e.target.files[0].name.split(".");
    const ext = parts[parts.length-1];
    if (ext === 'png' || ext === 'jpg' || ext === 'jpeg' || ext === 'bmp') {
        const r = new FileReader();
        r.onload = e => {
            const img = new Image();
            img.onload = ee => {
                const cv = document.createElement("canvas");
                cv.width = img.width > 384 ? 384 : img.width;
                cv.height = img.width > 384 ? 384/img.width * img.height : img.height;
                const ctx = cv.getContext("2d");
                ctx.drawImage(img, 0, 0, cv.width, cv.height);
                const image = toBitImg(ctx, cv.width, cv.height);

                const cv2 = document.createElement("canvas");
                cv2.width = cv.width;
                cv2.height = cv.height;
                const ctx2 = cv2.getContext("2d");
                drawBitImage(image, ctx2);
                images.push(image);

                const idx = images.length-1;

                cv2.addEventListener("click", () => {
                    document.querySelector("#input").value += '@'+idx.toString();
                    preview(parse(document.querySelector("#input").value));
                });

                document.querySelector("#img-list").appendChild(cv2);
            };

            img.src = e.target.result;
        };
        r.readAsDataURL(e.target.files[0]);
    } else {
        alert("invalid image file extension " + ext);
    }
});

async function printImageRow(bi, row) {
    const hd = bi.w > 384 ? 384 : bi.w;
    const vd = Math.min(24, bi.h - row*24);

    const hdbuf = new ArrayBuffer(2);
    const view16 = new Uint16Array(hdbuf);
    view16[0] = hd;
    const view8 = new Uint8Array(hdbuf);

    const buf = new Uint8Array(2 + hd*3);
    buf[0] = view8[0];
    buf[1] = view8[1];
    let bufI = 2;

    for(let x = 0; x < hd; x++) {
        for(let vb = 0; vb < 3; vb++) {
            if (vb < Math.ceil(vd / 8)) {
                const bd = Math.min(8, vd - vb*8);
                let data = 0;
                for(let dot = 0; dot < bd; dot++) {
                    data |= (bitAt(bi, x, row*24+vb*8+dot) ? 1 : 0) << dot;
                }

                data <<= (8-bd);
                buf[bufI++] = data;
            } else {
                buf[bufI++] = 0;
            }

        }
    }

    await sendCommand(0x10, buf);
}

async function printImage(bi) {
    const nLines = Math.ceil(bi.h / 24);
    for(let i = 0; i < nLines; i++) {
        await printImageRow(bi, i);
    }
}

async function print(txt) {
    const parts = parse(txt);

    for(let i = 0; i < parts.length; i++) {
        if (parts[i].type === 't') {
            await sendCommand(1, new Uint8Array([styleByte(parts[i])]));
            await sendCommand(0, strToBytes(parts[i].txt));
        } else if (parts[i].type == 'i') {
            await printImage(images[parts[i].i]);
        }
    }
}

document.querySelector("#print").addEventListener("click", () => {
   print(document.querySelector("#input").value);
});

power();