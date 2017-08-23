document.body.innerHTML += "awesome sauce"

document.getElementById("submit").onclick = e => {
  e.preventDefault()
  e.stopPropagation()
  console.log('yeah')
  const name = document.querySelector('[name=name]').value;
  fetch('/name', {
    body: name,
    method: 'POST',
  }).then(res => res.json())
  .then(res => console.log('got', res))
  .catch(err => console.log('fail', err))
}