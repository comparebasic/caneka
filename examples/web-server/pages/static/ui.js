(function(){
    const testsPage = document.getElementById('test-output');
    if(typeof testPage != 'undefined'){
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
    }
})();
(function(){
    const forms = document.getElementsByTagName('FORM');
    function getSetter(elem){
        const box = elem.getBoundingClientRect();
        return function setContent(resp){
            console.log(resp);
            const newElem = document.createElement("DIV");
            const d = new TextDecoder('utf-8');
            resp.body.getReader().read().then(content => {
                newElem.innerHTML = d.decode(content.value);
                newElem.style.height = box.height + 'px';
                elem.after(newElem);
                elem.remove();
            });
        }
    }
    if(typeof forms != 'undefined'){
        const length = forms.length;
        for(let i = 0; i < length; i++){ 
            forms[i].onsubmit = function(e){
                const form = e.currentTarget;
                e.preventDefault();
                let formData = {};
                const inputs = form.getElementsByTagName("INPUT");
                if(typeof inputs != 'undefined'){
                    const inpLength = inputs.length;
                    for(let j = 0; j < inpLength; j++){
                        const name = inputs[j].getAttribute('name');
                        if(name != null){
                            formData[name] = inputs[j].value;
                        }
                    }
                    console.log(formData);
                }
                if(Object.keys(formData).length > 0){
                    const headers = new Headers();
                    headers.append("Content-Type", "application/json");
                    headers.append("Accept", "text/html");
                    fetch(form.getAttribute("action"), {
                        method: "POST",
                        headers: headers,
                        body:JSON.stringify(formData)
                    }).then(getSetter(form)).catch(getSetter(form));

                }
                return false;
            }
        }
    }
})();
