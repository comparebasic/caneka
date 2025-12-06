(function(){
    if(typeof window.ui == 'undefined'){
        window.ui = {
            animCssProp: function(elem, propdefs, duration, cb){
                /* anim here */
                console.log(propdefs, elem);
                if(typeof elem._anim === 'undefined'){
                    elem._anim = {
                        timer: setInterval(window.ui.animCssProp.bind(window.ui, elem, propdefs, duration, cb), 100),
                        duration: duration,
                        start: Date.now()
                    }
                }
                const elapsed = Date.now() - elem._anim.start;
                console.log('_anim ' + elapsed + ' of ' + duration, elem._anim);
                for(let i = 0; i < propdefs.length; i++){
                    const def = propdefs[i];
                    let value = 0;
                    if(def[1] > def[2]){
                        value = def[1] + ((def[2] - def[1]) * (duration/elapsed));
                    }else{
                        value = def[2] + ((def[1] - def[2]) * (duration/elapsed));
                    }
                    elem.style[def[0]] = value;
                    console.log(def[0] + " " + value, elem);
                }

                if(elapsed >= duration){
                    console.log("DONE");
                    clearInterval(elem._anim);
                    delete elem._anim;
                    if(cb != null){
                        cb();
                    }
                }
            },
            resizeReplace: function(old, newElem){
                const position = window.getComputedStyle(newElem).getPropertyValue('position') || 'static';
                        console.log(position);
                newElem.style.opacity = 0.0;
                old.style.opacity = 1.0;
                newElem.style.position = 'relative';
                newElem.left = - 1024;
                old.after(newElem);

                const box = old.getBoundingClientRect();
                const newBox = newElem.getBoundingClientRect();

                const parNode = old.parentNode;
                if(box.height > newBox.height){
                    console.log("smaller " + box.height + " vs " + newBox.height);
                    window.ui.animCssProp(old, [["opacity", 1.0, 0.0],["height", box.height, newBox.height]], 1000, function(){
                        old.remove();
                        newElem.style.position = position;
                        newElem.style.left = 0;
                        window.ui.animCssProp(newElem, [["opacity", 0.1, 1.0]], 500); 
                    })
                }else{
                    console.log("larger " + newBox.height + " vs " + box.height);
                    window.ui.animCssProp(old, [["opacity", 1.0, 0.0]], 500, function(){
                        old.remove();
                        window.ui.animCssProp(old, [["height", old.height, newElem.height]], 1000, function(){
                            newElem.style.position = position;
                            newElem.style.left = 0;
                            window.ui.animCssProp(newElem, [["opacity", 0.0, 1.0]], 500, null); 
                        });
                    })
                }
            }
        };
    }
})();
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
        return function setContent(resp){
            console.log(resp);
            const newElem = document.createElement("DIV");
            const d = new TextDecoder('utf-8');
            resp.body.getReader().read().then(content => {
                newElem.innerHTML = d.decode(content.value);
                console.log(window.ui);
                window.ui.resizeReplace(elem, newElem);
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
