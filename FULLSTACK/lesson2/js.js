/*const a = "world";
a = 1;
*/

/*
let a = "Hello";
console.log(a);

function func() {
    a = 1;
}
func();
console.log(a);

function gunc() {
    let a = 2;
}
gunc();
console.log(a);

a = "qwe";
console.log(a);
*/

/*
//интересная задачка
console.log("Ba" + "c" * 10 + "a");
// что выведется?
*/

/*
console.log("w" + Math.E + "R" + typeof(1 * 2) + 1);
console.log("");

obj = {
    1: 2,
    4: 5
}

for (a in obj) {
    console.log(a);
}*/

/*
a = {1:1, 2:2, 3:3}
console.log(...a);

a = [1, 2, 3]

function f(b, c, d) {
    return b + c;
}

console.log(f(...a))
*/

/*
function func(x) {
    ans = 0;
    for (elem in x) {
        if (typeof(x[elem]) == typeof(1)) {
            ans += x[elem];
        }
    }
    return ans;
}

let goods = {
    name1: 'Арбуз',
    price1: 100,
    name2: 'Вишня',
    price2: 35,
    name3: 'Яблоко',
    price3: "45",
    name4: 'Клубника',
    price5: 250,
}

console.log(func(goods));
*/

/*
function countString(arr) {
    ans = 0;
    for (x in arr) {
        if (typeof(arr[x]) == "string") {
            ans++;
        }
    }
    return ans;
}

console.log(countString([1,true,'3','value1',9,'key']));
*/

/*
for (x in [5, 2, 3]) {
    if (typeof(arr[x]) == "string") {
        ans++;
    }
}
*/


// LEVEL 3 /////////////////////////////////////////////////////////////////////////////

/*
function alerted() {
    let newElem = document.createElement("div");
    newElem.innerText = "YYYYYYYYYYYYYYYYYYYYYYYYYY";
    newElem.className = "block";
    let parentElem = document.getElementById("myid");
    //parentElem.append(newElem);
    //parentElem.before(newElem);
    //parentElem.before(parentElem.cloneNode());
    for (let i = 0; i < 10; i += 1) {
        let cloned = newElem.cloneNode();
        cloned.innerText = "YYY" + i + "YYY";
        parentElem.before(cloned);
    }
    //parentElem.before(newElem);
}
*/

/*
let divElem = document.querySelector(".block");
divElem.style.background = "black";

document.getElementById("console").textContent = "aaa";

let allDivs = document.getElementsByTagName("div");
*/

/*
for (let i of allDivs) {
    i.hidden = "true";
    //i.style.display = "none";
}
*/
/*
for (; allDivs.length > 0;) {
    allDivs[0].remove();
}
*/

/*
let content = [
    {"id": 1, "title": "I", "text": "one"},
    {"id": 2, "title": "II", "text": "two"},
    {"id": 3, "title": "III", "text": "three"}
]

let parentElem = document.getElementById("myid");
parentElem.style = "display: grid; grid-gap: 5px; grid-template-columns: 1fr 1fr 1fr"

for (let oneData of content) {
    let newElem = document.createElement("div");
    newElem.id = oneData["id"];
    newElem.innerText = oneData["text"];
    newElem.style.backgroundColor = "red";
    //newElem.style.paddingBottom = "10vh";
    let newHeader = document.createElement("h2");
    newHeader.innerText = oneData["title"];
    newHeader.style.backgroundColor = "green";
    //newHeader.style.paddingTop = "10vh";
    let newImg = document.createElement("img");
    newImg.src = "http://www.opengl-tutorial.org/assets/images/tuto-3-matrix/perspective_red_triangle.png";
    newImg.style.maxWidth = "300px";
    newImg.addEventListener("mousedown", () => console.log("down"));
    newImg.addEventListener("mouseup", () => console.log("up"));
    newImg.addEventListener("click", () => console.log("click"));
    newImg.addEventListener("dblclick", function (e) {e.target.remove();});
    newImg.addEventListener("keypress", (e) => console.log("keypressed")); // affects children

    parentElem.appendChild(newElem);
    newElem.prepend(newHeader);
    newElem.append(newImg);
}
*/
