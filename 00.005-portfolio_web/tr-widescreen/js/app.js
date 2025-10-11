
const hamburger_menu = document.querySelector(".hamburger");
const wrap_div = document.querySelector(".main-screen");
const links_div = document.querySelectorAll(".links");

let anim_png = document.querySelector(".anim-png");
let target_Div = document.querySelector(".scroll-target"); // Получаем элемент, до которого нужно прокрутить

anim_png.addEventListener("click", () => {

    target_Div.scrollIntoView({
        behavior: 'smooth', // Плавная прокрутка
        block: 'start' // Прокрутка до верхней части элемента
    });

});

function closeMenu()
{
    wrap_div.classList.remove("open");
    document.body.classList.remove("stop-scrolling");
}

hamburger_menu.addEventListener("click", () => {

    if(!wrap_div.classList.contains("open"))
    {
        wrap_div.classList.add("open");
        document.body.classList.add("stop-scrolling");
    }
    else
    {
        closeMenu();
    }

});

links_div.forEach( link => link.addEventListener("click", () => closeMenu()));        

