$(document).on("click", ".aisocial", function (e) {
  var url = e.target.attributes['data-url'].value;
  setModalUrl(url);
});

function setModalUrl(url) {
  document.getElementById("aisocial").src = url;
  if(doc = document.getElementById("aisocial"))
    doc.height = $(window).height() * 0.85;
}
