/*
 *
 * This file gets all books information from Amazon API
 *
 */

const GOOGLE_BOOKS_API = 'https://www.googleapis.com/books/v1/volumes';

async function fetchBookData(query) {
  try {
    const response =
        await fetch(`${GOOGLE_BOOKS_API}?q=${encodeURIComponent(query)}`);
    const data = await response.json();

    if (data.totalItems === 0) {
      throw new Error('Livro não encontrado');
    }

    return data.items[0].volumeInfo;
  } catch (error) {
    console.error('Erro ao buscar dados do livro:', error);
    return null;
  }
}

async function getTitulo(isbn) {
  const bookData = await fetchBookData(`isbn:${isbn}`);
  return bookData ? bookData.title : null;
}

async function getAutor(isbn) {
  const bookData = await fetchBookData(`isbn:${isbn}`);
  return bookData ? bookData.authors?.join(', ') : null;
}

async function getIsbn(isbn) {
  const bookData = await fetchBookData(`isbn:${isbn}`);
  if (!bookData) return null;

  const identifiers = bookData.industryIdentifiers || [];
  for (const id of identifiers) {
    if (id.type === 'ISBN_13') return id.identifier;
    if (id.type === 'ISBN_10') return id.identifier;
  }

  return null;
}

async function getGenero(isbn) {
  const bookData = await fetchBookData(`isbn:${isbn}`);
  return bookData ? bookData.categories || [] : [];
}

async function getAnoPublicacao(isbn) {
  const bookData = await fetchBookData(`isbn:${isbn}`);
  if (!bookData || !bookData.publishedDate) return null;

  const year = bookData.publishedDate.split('-')[0];
  return parseInt(year);
}

async function getEditora(isbn) {
  const bookData = await fetchBookData(`isbn:${isbn}`);
  return bookData ? bookData.publisher : null;
}

async function getSinopse(isbn) {
  const bookData = await fetchBookData(`isbn:${isbn}`);
  return bookData ? bookData.description : null;
}

export async function buscarLivrosAPI(termo, maxResults = 10) {
  try {
    const response = await fetch(`${GOOGLE_BOOKS_API}?q=${
        encodeURIComponent(termo)}&maxResults=${maxResults}`);
    const data = await response.json();

    if (data.totalItems === 0) return [];

    return data.items.map(
        item => ({
          id: item.id,
          titulo: item.volumeInfo.title || 'Título desconhecido',
          autor: item.volumeInfo.authors?.join(', ') || 'Autor desconhecido',
          genero: item.volumeInfo.categories?.join(', ') ||
              'Gênero não especificado',
          ano: item.volumeInfo.publishedDate?.substring(0, 4) ||
              'Ano desconhecido',
          capa: item.volumeInfo.imageLinks?.thumbnail || '',
          editora: item.volumeInfo.publisher || 'Editora desconhecida'
        }));
  } catch (error) {
    console.error('Erro na busca de livros:', error);
    throw error;
  }
}

export default {
  getTitulo,
  getAutor,
  getIsbn,
  getGenero,
  getAnoPublicacao,
  getEditora,
  getSinopse,
  buscarLivrosAPI
};