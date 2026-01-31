(function(){
    if(typeof window.ui == 'undefined'){
        window.ui = {
            animCssProp: function(elem, propdefs, duration, cb){
                if(typeof elem._anim === 'undefined'){
                    elem._anim = {
                        timer: setInterval(
                            window.ui.animCssProp.bind(
                                window.ui, elem, propdefs, duration, cb), 15),
                        duration: duration,
                        start: Date.now()
                    }
                }

                const elapsed = Date.now() - elem._anim.start;
                for(let i = 0; i < propdefs.length; i++){
                    const def = propdefs[i];
                    const prop = def[0];
                    let value = 0;
                    if(def[1] > def[2]){
                        let pos = 1;
                        if(elapsed > 0){
                            pos = 1 - elapsed/duration;
                        }
                        value = def[2] - ((def[2] - def[1]) * pos);
                        if(value < def[2]){
                            value = def[2];
                        }
                    }else{
                        let pos = 0;
                        if(elapsed > 0){
                            pos = elapsed/duration;
                        }
                        value = def[1] + ((def[2] - def[1]) * pos);
                        if(value > def[2]){
                            value = def[2];
                        }
                    }

                    if(prop == 'height' || prop == 'width' || prop == 'padding' || 
                            prop == 'margin'){
                        elem.style[prop] = value + 'px';
                    }else{
                        elem.style[prop] = value;
                    }
                }

                if(elapsed >= duration){
                    clearInterval(elem._anim.timer);
                    delete elem._anim;
                    if(cb != null){
                        cb();
                    }
                }
            },
            resizeReplace: function(old, newElem){
                newElem.style.opacity = 0.0;
                old.style.opacity = 1.0;

                const par = old.parentNode;
                const box = par.getBoundingClientRect();
                par.style.height = box.height + 'px';
                par.style.overflow = 'hidden';
                old.after(newElem);

                const newBox = newElem.getBoundingClientRect();

                const parNode = old.parentNode;
                if(box.height > newBox.height){
                    window.ui.animCssProp(old.parentNode,[["height", box.height, newBox.height]], 500, function(){
                        window.ui.animCssProp(newElem, [["opacity", 0.1, 1.0]], 300, function(){ 
                            par.style.overflow = 'auto';
                            par.style.height = 'auto';
                        });
                    });
                    window.ui.animCssProp(old,[["opacity", 1.0, 0.0]], 200, function(){
                        old.remove();
                    });
                }else{
                    window.ui.animCssProp(old, [["opacity", 1.0, 0.0]], 300, function(){
                        old.remove();
                        window.ui.animCssProp(old.parentNode,
                                [["height", old.height, newElem.height]], 500, function(){
                            window.ui.animCssProp(newElem,
                                [["opacity", 0.0, 1.0]], 200, function(){ 
                                    par.style.overflow = 'auto';
                                    par.style.height = 'auto';
                                }); 
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
