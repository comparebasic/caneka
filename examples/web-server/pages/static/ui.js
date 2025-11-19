(function(){
    const testsPage = document.getElementById('page-tests');
    if(testsPage){
        console.log('wohoo test page', testsPage);
    }
    const h3s = testsPage.getElementsByTagName('H3');
    let length = h3s.length;
    for(let i = 0; i < length; i++){
        const label = document.createElement("LABEL");
        const checkbox = document.createElement("INPUT");
        checkbox.setAttribute("type", "checkbox");
        checkbox.classList.add("hide-show");
        checkbox.id = testsPage.id + '-cb-' + i;
        label.appendChild(document.createTextNode("show details"));
        label.setAttribute("for", checkbox.id);
        h3s[i].after(checkbox);
        h3s[i].after(label);
    }
})();
