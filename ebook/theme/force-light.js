/* Default to light on first visit only — do not lock theme (picker stays usable). */
(function () {
  try {
    if (!localStorage.getItem("mdbook-theme")) {
      localStorage.setItem("mdbook-theme", "light");
    }
  } catch (e) {
    /* ignore */
  }
})();
