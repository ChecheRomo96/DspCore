(function() {
    const page = window.location.pathname.split('/').pop();
    const collapsibleIndexes = ['annotated.html', 'topics.html'];

    if (collapsibleIndexes.includes(page) && typeof dynsection !== 'undefined') {
        // Keep namespaces/modules visible while collapsing their child entries.
        dynsection.toggleLevel(2);
    }
})();
