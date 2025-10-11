
let block_2_wrapper = document.querySelector(".block-1-wrapper");
let block_3_wrapper = document.querySelector(".block-2-wrapper");
let block_4_wrapper = document.querySelector(".block-3-wrapper");
let input_name = document.querySelector(".input-name");
let global_wrapper = document.querySelector(".global-wrapper");

let quiz_block_2 = document.querySelectorAll(".block1");

quiz_block_2.forEach((btn) => btn.addEventListener("click", () => {
    event.preventDefault();
    /*тут можна отримати ідентіфікатор опції із блоку 2*/
    console.log(btn.dataset.option);
    
    block_3_wrapper.classList.add("active");
}));

let quiz_block_3 = document.querySelectorAll(".block2");

quiz_block_3.forEach((btn) => btn.addEventListener("click", () => {
    event.preventDefault();
    /*тут можна отримати ідентіфікатор опції із блоку 3*/
    console.log(btn.dataset.option);

    block_4_wrapper.classList.add("active");

    //window.location.href = "./result.html";
}));

let quiz_block_4 = document.querySelectorAll(".block3");

quiz_block_4.forEach((btn) => btn.addEventListener("click", () => {
    event.preventDefault();
    /*тут можна отримати ідентіфікатор опції із блоку 3*/
    console.log(btn.dataset.option);

    global_wrapper.remove();

    const main = document.querySelector("main");
    main.classList.add("black-background")
    const div_item = document.createElement('div');
    div_item.classList.add("text_wrap");
    div_item.innerHTML = ' <img src="./img/animation.gif" alt="Описание GIF" class="centered-gif"><h1>ЗАМОВИТИ ТОВАР</h1> <h4>Ваш товар</h4> <a href="https://www.youtube.com/" class="btn-result">ЗАМОВИТИ ТОВАР</a>';
    main.appendChild(div_item);

    //window.location.href = "./result.html";
}));

let lang_btn = document.querySelector(".lang");

let lang_en = false;

function changeLang()
{
    if(!lang_en)
    {
        lang_btn.classList.remove("ukr-lang");
        lang_btn.classList.add("eng-lang");
        lang_en = true;
        //window.location.href = "./en/index.html";
        
    }
    else
    {
        lang_btn.classList.remove("eng-lang");
        lang_btn.classList.add("ukr-lang");
        lang_en = false;
        //window.location.href = "./ua/index.html";
        
    }
}

lang_btn.addEventListener("click", changeLang);