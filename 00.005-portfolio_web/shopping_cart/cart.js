
let cartItems;
let cartItemsList;
let totalPriceElement;

function update_elements()
{
    cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];
    cartItemsList = document.getElementById('cart-items');
    totalPriceElement = document.getElementById('total-price');
}


// Функция для отображения товаров в корзине
    function displayCartItems() {

        update_elements();

        cartItemsList.innerHTML = ''; // Очищаем текущий список товаров
        cartItems.forEach((item, index) => {
            const listItem = document.createElement('li');
            listItem.textContent = `${item.name} - ${item.price} руб. x ${item.quantity}`;
            
            // Создаем кнопку для удаления товара
            const deleteButton = document.createElement('button');
            deleteButton.textContent = 'Удалить';
            deleteButton.addEventListener('click', () => {
                removeItemFromCart(index); // Удаляем товар при нажатии
            });

            const deleteQuontityButton = document.createElement('button');
            deleteQuontityButton.textContent = '-';
            deleteQuontityButton.addEventListener('click', () => {
                removeQuantityItemFromCart(index); // Уменьшаем товар при нажатии
            });

            const addQuontityButton = document.createElement('button');
            addQuontityButton.textContent = '+';
            addQuontityButton.addEventListener('click', () => {
                addQuantityItemFromCart(index); // Уменьшаем товар при нажатии
            });

            listItem.appendChild(deleteQuontityButton); // Добавляем кнопку к элементу списка
            listItem.appendChild(addQuontityButton); // Добавляем кнопку к элементу списка
            listItem.appendChild(deleteButton); // Добавляем кнопку к элементу списка
            cartItemsList.appendChild(listItem); // Добавляем элемент списка в корзину
        });

        // Подсчитываем итоговую цену
        const totalPrice = cartItems.reduce((total, item) => total + (item.price * item.quantity), 0);
        totalPriceElement.textContent = `Итоговая цена: ${totalPrice} руб.`;

        updateCartCount();
    }


    // Функция для удаления товара из корзины
    function removeItemFromCart(index) {
        cartItems.splice(index, 1); // Удаляем товар из массива
        localStorage.setItem('cartItems', JSON.stringify(cartItems)); // Сохраняем обновленный массив в localStorage
        displayCartItems(); // Обновляем отображение товаров
    }

    // Очистить корзину
    document.getElementById('clear-cart').addEventListener('click', () => {
        localStorage.removeItem('cartItems');
        cartItemsList.innerHTML = '';
        totalPriceElement.textContent = 'Итоговая цена: 0 руб.';
        displayCartItems();
    });

    // Отображаем товары при загрузке страницы
    

    
    //функция уменьшает количество товара
    function removeQuantityItemFromCart(in_index)
    {
        // Получаем текущее содержимое корзины из localStorage
        let cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];

        // Проверяем, есть ли уже этот товар в корзине
        let existingItem = cartItems.find((item, index) => index == in_index);

        if (existingItem)
        {
            if(existingItem.quantity > 0)
            {
                existingItem.quantity -= 1; // Уменьшаем количество товара

                if (existingItem.quantity == 0)
                {
                    existingItem.quantity = 0;
                    cartItems.splice(in_index, 1); // Удаляем товар из массива
              
                }
            }
        }

        // Сохраняем обновленный список товаров в localStorage
        localStorage.setItem('cartItems', JSON.stringify(cartItems));

        // Обновляем счетчик товаров в корзине
        displayCartItems();

    }


        //функция добавляет количество товара
        function addQuantityItemFromCart(in_index)
        {
            
            // Получаем текущее содержимое корзины из localStorage
            let cartItems = JSON.parse(localStorage.getItem('cartItems')) || [];
    
            // Проверяем, есть ли уже этот товар в корзине
            let existingItem = cartItems.find((item, index) => index == in_index);
    
            if (existingItem)
            {
                    existingItem.quantity += 1; // Уменьшаем количество товара
            }
    
            // Сохраняем обновленный список товаров в localStorage
            localStorage.setItem('cartItems', JSON.stringify(cartItems));
    
            // Обновляем счетчик товаров в корзине
            displayCartItems();
    
        }