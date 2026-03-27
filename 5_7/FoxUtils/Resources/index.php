
<?php
/**
 * FoxUtils Documentation Viewer
 * Displays and searches the bundled FoxUtils_UserGuide.pdf
 *
 * Upload this file alongside FoxUtils_UserGuide.pdf to your web server.
 */

$pdf_file = 'FoxUtils_UserGuide.pdf';
$pdf_exists = file_exists($pdf_file);
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>FoxUtils v1.0 — Documentation</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }

        :root {
            --bg: #1a1410;
            --bg2: #241e17;
            --bg3: #2e261d;
            --gold: #c8a44e;
            --gold-dim: #a07e30;
            --text: #e8e0d4;
            --text-dim: #9a917f;
            --border: #3a3025;
        }

        body {
            font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;
            background: var(--bg);
            color: var(--text);
            min-height: 100vh;
            display: flex;
            flex-direction: column;
        }

        header {
            background: var(--bg2);
            border-bottom: 1px solid var(--border);
            padding: 16px 24px;
            display: flex;
            align-items: center;
            justify-content: space-between;
            flex-wrap: wrap;
            gap: 12px;
        }

        .logo {
            display: flex;
            align-items: center;
            gap: 12px;
        }

        .logo h1 {
            font-family: 'Georgia', serif;
            font-size: 1.4rem;
            color: var(--gold);
            font-variant: small-caps;
            letter-spacing: 1px;
        }

        .logo .version {
            background: var(--gold-dim);
            color: var(--bg);
            font-size: 0.7rem;
            font-weight: 700;
            padding: 2px 8px;
            border-radius: 4px;
            font-family: monospace;
        }

        nav {
            display: flex;
            gap: 8px;
        }

        nav a {
            color: var(--text-dim);
            text-decoration: none;
            font-size: 0.85rem;
            font-variant: small-caps;
            letter-spacing: 0.5px;
            padding: 4px 10px;
            border-radius: 4px;
            transition: all 0.2s;
        }

        nav a:hover, nav a.active {
            color: var(--gold);
            background: var(--bg3);
        }

        .toolbar {
            background: var(--bg2);
            border-bottom: 1px solid var(--border);
            padding: 10px 24px;
            display: flex;
            align-items: center;
            gap: 12px;
            flex-wrap: wrap;
        }

        .toolbar .search-box {
            display: flex;
            align-items: center;
            background: var(--bg3);
            border: 1px solid var(--border);
            border-radius: 6px;
            padding: 6px 12px;
            flex: 1;
            max-width: 400px;
            transition: border-color 0.2s;
        }

        .toolbar .search-box:focus-within {
            border-color: var(--gold-dim);
        }

        .toolbar .search-box svg {
            width: 16px;
            height: 16px;
            fill: var(--text-dim);
            flex-shrink: 0;
        }

        .toolbar .search-box input {
            background: none;
            border: none;
            outline: none;
            color: var(--text);
            font-size: 0.9rem;
            margin-left: 8px;
            width: 100%;
        }

        .toolbar .search-box input::placeholder {
            color: var(--text-dim);
        }

        .search-nav {
            display: none;
            align-items: center;
            gap: 6px;
            font-size: 0.8rem;
            color: var(--text-dim);
        }

        .search-nav.visible {
            display: flex;
        }

        .search-nav button {
            background: var(--bg3);
            border: 1px solid var(--border);
            color: var(--text);
            width: 28px;
            height: 28px;
            border-radius: 4px;
            cursor: pointer;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 0.8rem;
            transition: all 0.2s;
        }

        .search-nav button:hover {
            border-color: var(--gold-dim);
            color: var(--gold);
        }

        .btn {
            background: var(--bg3);
            border: 1px solid var(--border);
            color: var(--text);
            padding: 6px 14px;
            border-radius: 6px;
            font-size: 0.85rem;
            cursor: pointer;
            text-decoration: none;
            display: inline-flex;
            align-items: center;
            gap: 6px;
            transition: all 0.2s;
        }

        .btn:hover {
            border-color: var(--gold-dim);
            color: var(--gold);
        }

        .btn svg {
            width: 14px;
            height: 14px;
            fill: currentColor;
        }

        main {
            flex: 1;
            display: flex;
            justify-content: center;
            padding: 0;
            position: relative;
        }

        .pdf-container {
            width: 100%;
            height: calc(100vh - 110px);
        }

        .pdf-container canvas {
            display: block;
            margin: 0 auto;
        }

        #pdf-viewer {
            width: 100%;
            height: 100%;
            overflow: auto;
            padding: 20px 0;
        }

        .page-wrapper {
            position: relative;
            margin: 0 auto 16px;
            box-shadow: 0 2px 16px rgba(0,0,0,0.4);
        }

        .page-wrapper canvas {
            display: block;
        }

        .text-layer {
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            overflow: hidden;
            opacity: 0.3;
            line-height: 1;
        }

        .text-layer span {
            position: absolute;
            white-space: pre;
            color: transparent;
            cursor: text;
        }

        .text-layer .highlight {
            background: var(--gold);
            opacity: 0.4;
            border-radius: 2px;
        }

        .text-layer .highlight.active {
            opacity: 0.7;
        }

        .error-box {
            text-align: center;
            padding: 80px 24px;
        }

        .error-box h2 {
            color: var(--gold);
            font-family: 'Georgia', serif;
            margin-bottom: 12px;
        }

        .error-box p {
            color: var(--text-dim);
            max-width: 500px;
            margin: 0 auto;
            line-height: 1.6;
        }

        .error-box code {
            background: var(--bg3);
            padding: 2px 6px;
            border-radius: 3px;
            font-size: 0.9em;
        }

        .page-num {
            text-align: center;
            color: var(--text-dim);
            font-size: 0.75rem;
            padding: 4px 0 12px;
        }

        .loading {
            text-align: center;
            padding: 80px 24px;
            color: var(--text-dim);
        }

        .loading .spinner {
            width: 32px;
            height: 32px;
            border: 3px solid var(--border);
            border-top-color: var(--gold);
            border-radius: 50%;
            animation: spin 0.8s linear infinite;
            margin: 0 auto 16px;
        }

        @keyframes spin { to { transform: rotate(360deg); } }

        @media (max-width: 600px) {
            header { padding: 12px 16px; }
            .toolbar { padding: 8px 16px; }
            .logo h1 { font-size: 1.1rem; }
            .toolbar .search-box { max-width: 100%; }
        }
    </style>
</head>
<body>

<header>
    <div class="logo">
        <h1>FoxUtils</h1>
        <span class="version">v1.0</span>
    </div>
    <nav>
        <a href="#" class="active">Documentation</a>
        <a href="https://www.fab.com/sellers/AlchemyFoxStudio" target="_blank">Fab</a>
        <a href="mailto:alchemyfoxstudio@gmail.com">Contact</a>
    </nav>
</header>

<?php if ($pdf_exists): ?>
<div class="toolbar">
    <div class="search-box">
        <svg viewBox="0 0 24 24"><path d="M15.5 14h-.79l-.28-.27A6.47 6.47 0 0016 9.5 6.5 6.5 0 109.5 16c1.61 0 3.09-.59 4.23-1.57l.27.28v.79l5 4.99L20.49 19l-4.99-5zm-6 0C7.01 14 5 11.99 5 9.5S7.01 5 9.5 5 14 7.01 14 9.5 11.99 14 9.5 14z"/></svg>
        <input type="text" id="searchInput" placeholder="Search documentation... (Ctrl+K)" autocomplete="off">
    </div>
    <div class="search-nav" id="searchNav">
        <span id="searchCount">0/0</span>
        <button onclick="searchPrev()" title="Previous">&#9650;</button>
        <button onclick="searchNext()" title="Next">&#9660;</button>
    </div>
    <a href="<?= htmlspecialchars($pdf_file) ?>" download class="btn">
        <svg viewBox="0 0 24 24"><path d="M19 9h-4V3H9v6H5l7 7 7-7zM5 18v2h14v-2H5z"/></svg>
        Download PDF
    </a>
</div>

<main>
    <div class="pdf-container">
        <div id="pdf-viewer">
            <div class="loading">
                <div class="spinner"></div>
                Loading documentation...
            </div>
        </div>
    </div>
</main>

<script src="https://cdnjs.cloudflare.com/ajax/libs/pdf.js/4.9.155/pdf.min.mjs" type="module"></script>
<script type="module">
    import * as pdfjsLib from 'https://cdnjs.cloudflare.com/ajax/libs/pdf.js/4.9.155/pdf.min.mjs';

    pdfjsLib.GlobalWorkerOptions.workerSrc = 'https://cdnjs.cloudflare.com/ajax/libs/pdf.js/4.9.155/pdf.worker.min.mjs';

    const viewer = document.getElementById('pdf-viewer');
    const searchInput = document.getElementById('searchInput');
    const searchNav = document.getElementById('searchNav');
    const searchCount = document.getElementById('searchCount');

    let pdfDoc = null;
    let pages = [];
    let textContents = [];
    let searchMatches = [];
    let currentMatch = -1;
    let scale = 0;
    let searchTimeout = null;

    function getScale(page) {
        const vp = page.getViewport({ scale: 1 });
        const containerWidth = viewer.clientWidth - 40;
        return Math.min(containerWidth / vp.width, 2.0);
    }

    async function loadPDF() {
        try {
            pdfDoc = await pdfjsLib.getDocument('<?= htmlspecialchars($pdf_file) ?>').promise;
            viewer.innerHTML = '';

            for (let i = 1; i <= pdfDoc.numPages; i++) {
                const page = await pdfDoc.getPage(i);
                if (scale === 0) scale = getScale(page);
                const viewport = page.getViewport({ scale });

                const wrapper = document.createElement('div');
                wrapper.className = 'page-wrapper';
                wrapper.style.width = viewport.width + 'px';
                wrapper.style.height = viewport.height + 'px';

                const canvas = document.createElement('canvas');
                canvas.width = viewport.width * window.devicePixelRatio;
                canvas.height = viewport.height * window.devicePixelRatio;
                canvas.style.width = viewport.width + 'px';
                canvas.style.height = viewport.height + 'px';

                const ctx = canvas.getContext('2d');
                ctx.scale(window.devicePixelRatio, window.devicePixelRatio);
                wrapper.appendChild(canvas);

                const textLayer = document.createElement('div');
                textLayer.className = 'text-layer';
                wrapper.appendChild(textLayer);

                viewer.appendChild(wrapper);

                const pageNum = document.createElement('div');
                pageNum.className = 'page-num';
                pageNum.textContent = `Page ${i} of ${pdfDoc.numPages}`;
                viewer.appendChild(pageNum);

                await page.render({ canvasContext: ctx, viewport }).promise;

                const textContent = await page.getTextContent();
                textContents.push({ content: textContent, viewport, layer: textLayer });

                textContent.items.forEach(item => {
                    const tx = pdfjsLib.Util.transform(viewport.transform, item.transform);
                    const span = document.createElement('span');
                    span.textContent = item.str;
                    span.style.left = tx[4] + 'px';
                    span.style.top = (tx[5] - item.height) + 'px';
                    span.style.fontSize = item.height + 'px';
                    span.style.fontFamily = item.fontName || 'sans-serif';
                    textLayer.appendChild(span);
                });

                pages.push({ page, wrapper, canvas, viewport });
            }
        } catch (e) {
            viewer.innerHTML = `<div class="error-box"><h2>Could not load PDF</h2><p>${e.message}</p></div>`;
        }
    }

    function doSearch(query) {
        searchMatches = [];
        currentMatch = -1;

        textContents.forEach(({ layer }) => {
            layer.querySelectorAll('.highlight').forEach(el => el.classList.remove('highlight', 'active'));
        });

        if (!query || query.length < 2) {
            searchNav.classList.remove('visible');
            return;
        }

        const q = query.toLowerCase();

        textContents.forEach(({ layer }, pageIdx) => {
            const spans = layer.querySelectorAll('span');
            spans.forEach(span => {
                const text = span.textContent.toLowerCase();
                if (text.includes(q)) {
                    span.classList.add('highlight');
                    searchMatches.push({ pageIdx, span });
                }
            });
        });

        searchNav.classList.add('visible');

        if (searchMatches.length > 0) {
            currentMatch = 0;
            highlightCurrent();
        }

        updateSearchCount();
    }

    function highlightCurrent() {
        searchMatches.forEach(m => m.span.classList.remove('active'));
        if (currentMatch >= 0 && currentMatch < searchMatches.length) {
            const m = searchMatches[currentMatch];
            m.span.classList.add('active');
            m.span.scrollIntoView({ behavior: 'smooth', block: 'center' });
        }
    }

    function updateSearchCount() {
        if (searchMatches.length === 0) {
            searchCount.textContent = '0 results';
        } else {
            searchCount.textContent = `${currentMatch + 1}/${searchMatches.length}`;
        }
    }

    window.searchNext = function() {
        if (searchMatches.length === 0) return;
        currentMatch = (currentMatch + 1) % searchMatches.length;
        highlightCurrent();
        updateSearchCount();
    };

    window.searchPrev = function() {
        if (searchMatches.length === 0) return;
        currentMatch = (currentMatch - 1 + searchMatches.length) % searchMatches.length;
        highlightCurrent();
        updateSearchCount();
    };

    searchInput.addEventListener('input', () => {
        clearTimeout(searchTimeout);
        searchTimeout = setTimeout(() => doSearch(searchInput.value.trim()), 250);
    });

    searchInput.addEventListener('keydown', (e) => {
        if (e.key === 'Enter') {
            e.shiftKey ? window.searchPrev() : window.searchNext();
            e.preventDefault();
        }
        if (e.key === 'Escape') {
            searchInput.value = '';
            doSearch('');
            searchInput.blur();
        }
    });

    document.addEventListener('keydown', (e) => {
        if ((e.ctrlKey || e.metaKey) && e.key === 'k') {
            e.preventDefault();
            searchInput.focus();
        }
    });

    loadPDF();
</script>

<?php else: ?>
<main>
    <div class="error-box">
        <h2>PDF Not Found</h2>
        <p>Place <code>FoxUtils_UserGuide.pdf</code> in the same directory as this file and reload the page.</p>
    </div>
</main>
<?php endif; ?>

</body>
</html>
