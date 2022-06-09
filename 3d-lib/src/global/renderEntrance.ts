import { App } from "../core/App";

function globeRenderEntrance() {
    App.getInstance().render();
    let globeRenderId = requestAnimationFrame(globeRenderEntrance);
}

globeRenderEntrance();