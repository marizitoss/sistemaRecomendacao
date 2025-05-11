import {buscarLivrosAPI} from './amazonBooksApi.js';

document.getElementById('searchButton').addEventListener('click', buscarLivros);
document.getElementById('searchInput').addEventListener('keypress', (e) => {
  if (e.key === 'Enter') buscarLivros();
});

async function buscarLivros() {
  const termo = document.getElementById('searchInput').value.trim();
  const carousel = document.getElementById('carousel');
  carousel.innerHTML = '<div class="loading">Buscando livros...</div>';

  if (!termo) {
    carousel.innerHTML =
        '<div class="message">Digite um termo para buscar</div>';
    return;
  }

  try {
    const livros = await buscarLivrosAPI(termo);
    exibirLivros(livros);
  } catch (error) {
    console.error('Erro ao buscar livros:', error);
    carousel.innerHTML =
        '<div class="message error">Erro ao buscar livros</div>';
  }
}

function exibirLivros(livros) {
  const carousel = document.getElementById('carousel');
  carousel.innerHTML = '';

  if (livros.length === 0) {
    carousel.innerHTML = '<div class="message">Nenhum livro encontrado</div>';
    return;
  }

  livros.forEach(livro => {
    carousel.appendChild(criarCardLivro(livro));
  });
}

function criarCardLivro(livro) {
  const card = document.createElement('div');
  card.className = 'book-card';
  card.innerHTML = `
    <div class="book-cover" style="${
      livro.capa ? `background-image: url('${livro.capa}')` :
                   'background-color: #f5f5f5'}">
      ${!livro.capa ? '<span class="no-cover">Sem capa</span>' : ''}
    </div>
    <div class="book-details">
      <h3 class="book-title">${livro.titulo}</h3>
      <p class="book-author">${livro.autor}</p>
      <div class="book-meta">
        <span class="book-year">${livro.ano}</span>
        ${
      livro.genero ? `<span class="book-genre">${livro.genero}</span>` : ''}
      </div>
    </div>
  `;
  return card;
}

// Busca inicial
buscarLivros('literatura brasileira');
