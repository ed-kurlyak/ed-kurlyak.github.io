var toggle_btn;
var big_wrapper;
var hamburger_menu;

toggle_btn = document.querySelector(".toggle-btn");
big_wrapper = document.querySelector(".big-wrapper");
hamburger_menu = document.querySelector(".hamburger-menu");

let dark = false;

function toggleAnimation()
{
    dark = !dark;
    if(dark)
    {
        big_wrapper.classList.remove("light");
        big_wrapper.classList.add("dark");
    }
    else
    {
        big_wrapper.classList.remove("dark");
        big_wrapper.classList.add("light");
    }
}

toggle_btn.addEventListener("click", toggleAnimation);
hamburger_menu.addEventListener("click", () => {
    big_wrapper.classList.toggle("active");
});
