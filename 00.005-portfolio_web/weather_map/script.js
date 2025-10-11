const apiKey = '593d5b6149d498a0bf8f0a05a9b53371'; // Замініть своїм ключем API OpenWeatherMap

// Функція пошуку погоди по місту
async function searchWeather() {
    const city = document.getElementById('city-input').value;
    if (!city) return alert('Please enter a city name.');

    try {
        const response = await fetch(`https://api.openweathermap.org/data/2.5/weather?q=${city}&appid=${apiKey}&units=metric&lang=en`);
        const data = await response.json();

        if (data.cod === 200) {
            displayWeather(data);
            addFavorite(city);
        } else {
            alert('City not found.');
        }
    } catch (error) {
        alert('Error fetching weather data.');
    }
}

// Функція відображення інформації про погоду
function displayWeather(data) {
    const weatherInfo = document.getElementById('weather-info');
    weatherInfo.innerHTML = `
        <h3>${data.name}</h3>
        <p>Temperature: ${data.main.temp} °C</p>
        <p>Humidity: ${data.main.humidity}%</p>
        <p>Wind Speed: ${data.wind.speed} m/s</p>
    `;
}

// Функція додавання міста до вибраного
function addFavorite(city) {
    let favorites = JSON.parse(localStorage.getItem('favorites')) || [];
    if (!favorites.includes(city)) {
        favorites.push(city);
        localStorage.setItem('favorites', JSON.stringify(favorites));
        renderFavorites();
    }
}

// Функція відтворення списку улюблених міст
function renderFavorites() {
    const favoritesList = document.getElementById('favorites-list');
    
    const favorites = JSON.parse(localStorage.getItem('favorites')) || [];
    favoritesList.innerHTML = '';
    

    favorites.forEach(city => {
        // Відобразити список вибраного
        const li = document.createElement('li');
        li.innerHTML = `
        <a href="#" onclick="selectFavoriteCity('${city}')">${city}</a>
            <button class="remove-btn" onclick="removeFavorite('${city}')">Remove</button>
        `;
        favoritesList.appendChild(li);

    });
}

// Функція видалення міста з вибраного
function removeFavorite(city) {
    let favorites = JSON.parse(localStorage.getItem('favorites')) || [];
    favorites = favorites.filter(fav => fav !== city);
    localStorage.setItem('favorites', JSON.stringify(favorites));
    renderFavorites();
}

// Функція отримання та відображення погоди для обраного улюбленого міста
async function selectFavoriteCity(city) {
    
    if (!city) return;

    try {
        const response = await fetch(`https://api.openweathermap.org/data/2.5/weather?q=${city}&appid=${apiKey}&units=metric&lang=en`);
        const data = await response.json();

        if (data.cod === 200) {
            displayWeather(data);
        } else {
            alert('City not found.');
        }
    } catch (error) {
        alert('Error fetching weather data.');
    }
}

// Відображати вибране під час завантаження сторінки
document.addEventListener('DOMContentLoaded', renderFavorites);
