var big_wrapper = document.querySelector(".big-wrapper");
var hamburger_menu = document.querySelector(".hamburger-menu");

hamburger_menu.addEventListener("click", () => {
	big_wrapper.classList.toggle("active");
	//document.body.classList.toggle("stop-scrolling");
});