/**
 * @page magic-pixels Magic Pixels
 * 
 * @tableofcontents
 *
 * @section magic-pixels-criteria Magic Pixel Criteria
 * A magic pixel is a pixel which has the same RED and BLUE values. 
 * This includes black, white, grays, purples, and greens. From this criteria,
 * there are @f$255 \times 255 \times 255 = 16581375@f$ possible magic pixels that you can use
 * (Pick a green, pick an alpha, and pick a red-blue). Magic pixels are less than
 * 0.4% of all the possible pixel colors you can use.
 *
 * @section magic-pixels-using How to use 
 * When a sprite has magic pixels and is being tagged as recolorable,
 * Cytopia will be allowed to recolor those pixels at runtime. This can be
 * done randomly, or specifically depending on for what your asset is being
 * used. Randomly refers to picking a random color and recoloring all magic pixels
 * with it, while specifically refers to using a specific color.
 * 
 * @section magic-pixels-recoloring Recoloring algorithm
 * Recoloring is done in the following manner:
 * Given a sprite of RGBA pixels, and a target RGBA color,
 * we find all pixels that are "magic" according to the @ref magic-pixels-criteria.
 * For each of these magic pixels, we apply the following filter
 * @f{align*}{
 *  H_o &= H_t\\
 *  S_o &= \mathrm{overlay}(S_i, S_t)\\
 *  L_o &= \mathrm{overlay}(L_i, L_t)\\
 * @f}
 * Where @f$H_o, S_o, L_o@f$ are the resulting hue, saturation, and lightness,
 * @f$H_i, S_i, L_i@f$ are the magic pixel's hue, saturation, and lightness,
 * @f$H_t, S_t, L_t@f$ are the target's hue, saturation, and lightness,
 * and we define @f{align*}{
 *  \mathrm{overlay}(X, Y) &= \begin{cases}
 *  \frac{2}{100}XY & \mathrm{if } X < 50\\
 *  100 - \frac{2}{100}(100-X)(100-Y) & \mathrm{otherwise}
 *  \end{cases}
 * @f}
 * With RGB, we derive the following.
 * Given @f$X_{min}=\min\{R_o,G_o,B_o\}@f$ and @f$X_{max}=\max\{R_o,G_o,B_o\}@f$, we have that
 * @f{align*}{
 *  L_o &= \mathrm{mid}\{R_o,G_o,B_o\} = \mathrm{overlay}\left(\mathrm{mid}\{R_i, G_i, B_i\}, \mathrm{mid}\{R_t,G_t,B_t\}\right)\\
 *  X_{max} + X_{min} &= 2L_o & (1)\\
 *  S_o &= \frac{X_{max}-X_{min}}{\min\{L_o, 1-L_o\}} = \mathrm{overlay}\left(
 *  \frac{\max\{R_i,G_i,B_i\} - \min\{R_i,G_i,B_i\}}{\min\{\mathrm{mid}\{R_i,G_i,B_i\}, 1-\mathrm{mid}\{R_i,G_i,B_i\}\}},
 *  \frac{\max\{R_t,G_t,B_t\} - \min\{R_t,G_t,B_t\}}{\min\{\mathrm{mid}\{R_t,G_t,B_t\}, 1-\mathrm{mid}\{R_t,G_t,B_t\}\}},
 *  \right)\\
 *  X_{max}-X_{min} &=S_o\min\{L_o, 1-L_o\} & (2)\\
 * @f}
 * From Eqn (1), you can see that @f$L_o=0\implies X_{max}=-X_{min}=0@f$, 
 * hence if there is no lightness, we always return black.
 * From Eqn (2), you can see that @f$S_o=0\implies X_{max}=X_{min}=L_o@f$,
 * hence if there is no saturation, we always return a grayscale color.
 * Note that if any denominator in the @f$S_o@f$ equation is 0, then
 * @f$S_o=0@f$ to prevent dividing by 0. Finally for hue, there are many ways to convert
 * it back to RGB.
 * @htmlonly[block]
 *  <style type="text/css">
 *  .pixel-block {
 *    width: 100px;
 *    height: 100px;
 *    display: inline-block;
 *    box-sizing: border-box;
 *    border: 1px solid hsl(0, 0%, 80%);
 *    border-radius: 5px;
 *  }
 *  .pixel-block-text {
 *    line-height: 20px;
 *    vertical-align: middle;
 *  }
 *  .pixel-section {
 *    width: 100px;
 *    height: 130px;
 *    display: inline-block;
 *    vertical-align: middle;
 *  }
 *  .pixel-section > input {
 *    width: 100px;
 *    box-sizing: border-box;
 *    margin-top: 5px;
 *    font-family: 'Inter', sans-serif;
 *  }
 *  .pixel-section > input[error="true"] {
 *    border-color: red;
 *  }
 *  #magic-pixels-example {
 *    box-sizing: border-box;
 *    padding: 10px;
 *  }
 *  #magic-pixels-example > span {
 *    font-size: 20px;
 *    font-weight: 300;
 *  }
 *  #magic-color-picker {
 *    margin-top: 5px;
 *  }
 *  </style>
 *  <script type="text/javascript">
 *  function overlay(X, Y) {
 *    if(X < 255 / 2) return 2 * X * Y / 255;
 *    return 255 - 2 * (255 - X) * (255 - Y) / 255;
 *  }
 *  function recolor_magic(magic, target) {
 *    // Not a magic pixel 
 *    if(magic[0] != magic[2]) return magic;
 *    var min_i = Math.min(...magic);
 *    var max_i = Math.max(...magic);
 *    var mid_i = (max_i + min_i) / 2;
 *    var min_t = Math.min(...target);
 *    var max_t = Math.max(...target);
 *    var mid_t = (max_t + min_t) / 2;
 *    var lo = overlay(mid_t, mid_i);
 *    if(lo == 0) return [0, 0, 0];
 *    console.log("Midi: ", mid_i, " Midt: ", mid_t);
 *    if(mid_i == 0 || mid_i == 255 || mid_t == 0 || mid_t == 255) return [Math.floor(lo), Math.floor(lo), Math.floor(lo)];
 *    console.log("1: ", Math.floor(255 * (max_i - min_i) / Math.min(mid_i, 255-mid_i) / 2));
 *    console.log("2: ", Math.floor(255 * (max_t - min_t) / Math.min(mid_t, 255-mid_t) / 2));
 *    var so = overlay(Math.floor(255 * (max_t - min_t) / Math.min(mid_t, 255-mid_t) / 2), Math.floor(255 * (max_i - min_i) / Math.min(mid_i, 255-mid_i) / 2));
 *    console.log("sat: ", so);
 *    if(so == 0) return [Math.floor(lo), Math.floor(lo), Math.floor(lo)];
 *    var it = target.indexOf(max_t);
 *    var ho = (60 * (2 * it + (target[(it+1)%3] - target[(it+2)%3])/(max_t-min_t)) + 360) % 360;
 *    console.log("hue: ", ho);
 *    return [
 *      Math.floor(lo - so * Math.min(lo, 255-lo) * Math.max(-1, Math.min((0+ho/30)%12 - 3, 9 - (0+ho/30)%12, 1)) / 255),
 *      Math.floor(lo - so * Math.min(lo, 255-lo) * Math.max(-1, Math.min((8+ho/30)%12 - 3, 9 - (8+ho/30)%12, 1)) / 255),
 *      Math.floor(lo - so * Math.min(lo, 255-lo) * Math.max(-1, Math.min((4+ho/30)%12 - 3, 9 - (4+ho/30)%12, 1)) / 255)
 *    ];
 *  }
 *  function get_color(element, color) {
 *    var match = element.value.match(/^#([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})$/);
 *    if(!match) {
 *      element.setAttribute("error", "true");
 *      throw "Invalid color string";
 *    }
 *    element.setAttribute("error", "false");
 *    color.style['background-color'] = element.value;
 *    return [ 
 *      parseInt(match[1], 16),
 *      parseInt(match[2], 16),
 *      parseInt(match[3], 16)
 *    ]
 *  }
 *  function magic_pixel_update() {
 *    try {
 *      color_magic = get_color(document.getElementById("magic-pixel-value"), 
 *          document.getElementById("magic-pixel"));
 *      color_target = get_color(document.getElementById("target-pixel-value"),
 *          document.getElementById("target-pixel"));
 *      console.log("Magic: ", color_magic);
 *      console.log("Target: ", color_target);
 *      result_color = recolor_magic(color_magic, color_target);
 *      console.log("Result: ", result_color);
 *      result_color = "#" 
 *      + ("00" + result_color[0].toString(16)).substr(-2) 
 *      + ("00" + result_color[1].toString(16)).substr(-2)
 *      + ("00" + result_color[2].toString(16)).substr(-2);
 *      document.getElementById("result-pixel-value").value = result_color;
 *      document.getElementById("result-pixel").style['background-color'] = result_color;
 *    } catch(e) {
 *      console.error(e);
 *      return;
 *    }
 *  }
 *  </script>
 *  <div id="magic-pixels-example">
 *    <span>Chose some colors to see how magic coloring works</span>
 *    <center>
 *    <div id="magic-color-picker">
 *      <div class="pixel-section">
 *        <div id="magic-pixel" class="pixel-block" style="background-color: #000000;"></div>
 *        <br />
 *        <input id="magic-pixel-value" value="#000000", onchange="magic_pixel_update(this)" />
 *      </div>
 *      <span class="pixel-block-text">+</span>
 *      <div class="pixel-section">
 *        <div id="target-pixel" class="pixel-block" style="background-color: #FF0000;"></div>
 *        <br />
 *        <input id="target-pixel-value" value="#FF0000" onchange="magic_pixel_update(this)" />
 *      </div>
 *      <span class="pixel-block-text">=</span>
 *      <div class="pixel-section">
 *        <div id="result-pixel" class="pixel-block" style="background-color: #000000;"></div>
 *        <br />
 *        <input id="result-pixel-value" value="#000000" readonly/>
 *      </div>
 *    </div>
 *    </center>
 *  </div>
 * @endhtmlonly
 *
 */
