(function() {
    function initialize() {
        const page = window.location.pathname.split('/').pop();
        const collapsibleIndexes = ['annotated.html', 'topics.html'];
        if (collapsibleIndexes.includes(page) && typeof dynsection !== 'undefined') {
            dynsection.toggleLevel(2);
        }
        if (page === 'annotated.html') {
            const contents = document.querySelector('.contents');
            if (contents && !document.getElementById('dspcore-api-index-note')) {
                const note = document.createElement('p');
                note.id = 'dspcore-api-index-note';
                note.appendChild(document.createTextNode('This index lists classes only. For interpolation, generators and all module APIs, open '));
                const link = document.createElement('a');
                link.href = 'topics.html';
                link.textContent = 'Topics: complete module index';
                note.appendChild(link);
                note.appendChild(document.createTextNode('.'));
                contents.insertBefore(note, contents.firstChild);
            }
        }
    }
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initialize);
    } else {
        initialize();
    }
})();
