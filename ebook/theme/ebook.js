/* Offline Mermaid for file:// — expects theme/mermaid.min.js loaded first. */
(function () {
  function decodeEntities(text) {
    var ta = document.createElement("textarea");
    ta.innerHTML = text;
    return ta.value;
  }

  function upgradeMermaidBlocks() {
    var nodes = document.querySelectorAll(
      "code.language-mermaid, pre > code.mermaid, code.mermaid"
    );
    nodes.forEach(function (code) {
      var pre = code.parentElement;
      var div = document.createElement("div");
      div.className = "mermaid";
      var src = code.textContent || decodeEntities(code.innerHTML);
      div.textContent = src.replace(/\u2013/g, "--").replace(/\u2014/g, "--");
      if (pre && pre.tagName === "PRE") {
        pre.parentNode.replaceChild(div, pre);
      } else {
        code.parentNode.replaceChild(div, code);
      }
    });
  }

  function fixBrokenDivMermaid() {
    /* Legacy staged pages: mdBook markdown-parsed inside <div class="mermaid"> */
    document.querySelectorAll("div.mermaid").forEach(function (div) {
      if (div.querySelector("svg")) return;
      var raw = div.textContent || "";
      if (!raw.trim()) return;
      raw = raw.replace(/\u2013/g, "--").replace(/\u2014/g, "--");
      div.textContent = raw;
    });
  }

  function run() {
    if (typeof mermaid === "undefined") return;
    upgradeMermaidBlocks();
    fixBrokenDivMermaid();
    try {
      mermaid.initialize({
        startOnLoad: false,
        theme: "neutral",
        securityLevel: "loose",
      });
      mermaid.run({ querySelector: ".mermaid" });
    } catch (e) {
      console.warn("mermaid:", e);
    }
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", run);
  } else {
    run();
  }
})();
