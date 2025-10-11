
let cart_html = document.querySelector(".cart-html");
let cart_right = document.querySelector(".cart-right");
let cart_icon = document.querySelector(".cart-icon");

/* Корзина выезжает справа или прячется */
function openCart()
{
    cart_right.classList.toggle("active");
    cart_icon.classList.toggle("active");
}

/* Если клик на иконке корзины справа вверху экрана */
cart_html.addEventListener("click", openCart)

document.querySelectorAll('.add-to-cart').forEach(button => {
    button.addEventListener('click', () => {
        const product = button.parentElement;
        const productName = product.getAttribute('data-name');
        const productPrice = parseFloat(product.getAttribute('data-price'));

        // Получаем текущее содержимое корзины из localStorage
        let cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];

        // Проверяем, есть ли уже этот товар в корзине
        const existingItem = cartItems.find(item => item.name === productName);
        if (existingItem) {
            existingItem.quantity += 1; // Увеличиваем количество товара
        } else {
            cartItems.push({ name: productName, price: productPrice, quantity: 1 });
        }

        // Сохраняем обновленный список товаров в localStorage
        localStorage.setItem('cartItems', JSON.stringify(cartItems));

        // Обновляем счетчик товаров в корзине
        displayCartItems();
        
    });
});

function updateCartCount() {
    const cartCount = document.getElementById('cart-count');
    const itemsCount = JSON.parse(localStorage.getItem('cartItems')) ? 
                       JSON.parse(localStorage.getItem('cartItems')).reduce((sum, item) => sum + item.quantity, 0) : 0;
    cartCount.textContent = itemsCount;
}

// Обновляем счетчик при загрузке страницы
// И список товаров в корзине
document.addEventListener('DOMContentLoaded', () => {
    displayCartItems();
    
});

// Обработчик для кнопки "Очистить корзину" на главной странице
document.getElementById('clear-cart-index').addEventListener('click', () => {
    localStorage.removeItem('cartItems'); // Удаляем корзину из localStorage
    displayCartItems();
});
