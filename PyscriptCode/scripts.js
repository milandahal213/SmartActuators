const divider = document.getElementById('divider');
const left = document.getElementById('left');
const right = document.getElementById('right');
const container = document.getElementById('container');

fetch('crumbs.html')
            .then(response => response.text())
            .then(html => {document.getElementById('breadcrumb-container').innerHTML = html;});

divider.addEventListener('mousedown', () => {
    document.addEventListener('mousemove', resize);
    document.addEventListener('mouseup', () => {
        document.removeEventListener('mousemove', resize);
        });
    });

function resize(e) {
    const containerWidth = container.offsetWidth;
    const leftWidth = e.clientX / containerWidth * 100;
    left.style.flex = leftWidth;
    right.style.flex = 100 - leftWidth;
}

function toggleContent(id) {
    document.getElementById('start').style.display = 'none';
    document.getElementById('element').style.display = 'none';
    document.getElementById('channel').style.display = 'none';
    document.getElementById('code').style.display = 'none';
    document.getElementById(id).style.display = 'block';
}

