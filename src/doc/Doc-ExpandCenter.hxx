/**
 * @page expand-center Expand Center Sprites
 *
 * @tableofcontents
 *
 * @section expand-center-idea Idea
 * The sprites we create have a fixed width and height. This is 
 * an issue because we want our user interface to be resizable and
 * fit many different dimension layouts. In order to support this,
 * we need the ability to resize sprites without scaling them.
 * This can be done with an expansion algorithm. The main idea is that we
 * will take the middle cross of the sprite and repeat those pixels 
 * continuously where expanding would lead to missing pixels.
 *
 * @section expand-center-algorithm Algorithm
 * Let @f$O:n\times m@f$ be a buffer of pixels of the original sprite and
 * let @f$D:a\times b@f$ be the destination buffer. The generator "cross"
 * are pixels @f[
 * C=\left\{O[x,y]\mid 
 *    x=\left\lfloor\frac{m}{2}\right\rfloor\vee y=\left\lfloor\frac{n}{2}\right\rfloor\right\}
 * @f]
 *
 * Let's define the projection of a pixel on the generating cross as:@f[
 *  \Phi(x, x_{min}, x_{max}) = \begin{cases}
 *    x & \mathrm{if}\ x\leq x_{min}\\
 *    x_{min} & \mathrm{if}\ x_{min}< x\leq x_{max}\\
 *    x - x_{max} + x_{min} & \mathrm{otherwise}
 *  \end{cases}
 * @f]
 *
 * Then we map the pixels in the following way: @f[
 * D[i,j]= O\left[\Phi\left(i, \left\lfloor\frac{n}{2}\right\rfloor, a-\left\lfloor\frac{n}{2}\right\rfloor\right), \Phi\left(j,\left\lfloor\frac{m}{2}\right\rfloor,\left\lfloor\frac{m}{2}\right\rfloor\right)\right]
 * @f]
 *
 * @section expand-center-example Example
 * Upload your sprite to test the algorithm.
 * @htmlonly[block]
 * <input type="file" id="Upload-Expand-Center" onchange="UploadExpandCenterPicture()" /><br />
 * Width: <input id="ExpandWidth" value="100" onchange="UploadExpandCenterPicture()" /><br />
 * Height: <input id="ExpandHeight" value="100" onchange="UploadExpandCenterPicture()" /><br />
 * Scale: <input id="ExpandRadius" value="5" onchange="UploadExpandCenterPicture()" /><br />
 * <center>
 * <canvas id="ExpandedImage" width="0" height="0" /></center>
 * <script>
 *
 *  var canvas = document.getElementById("ExpandedImage");
 *  var ctx = canvas.getContext("2d");
 *  var imageData, expand_width, expand_height, expand_radius;
 *
 *
 * function UploadExpandCenterPicture() {
 *  file = document.getElementById("Upload-Expand-Center").files[0];
 *  expand_width = Number(document.getElementById("ExpandWidth").value);
 *  expand_height = Number(document.getElementById("ExpandHeight").value);
 *  expand_radius = Number(document.getElementById("ExpandRadius").value);
 *  if(FileReader) {
 *    let fr = new FileReader();
 *    fr.onload = () => showImage(fr);
 *    fr.readAsDataURL(file);
 *  }
 * }
 *
 * function showImage(fileReader) {
 *  let img = new Image();
 *  img.onload = () => getImageData(img);
 *  img.src = fileReader.result;
 * }
 *
 * function getImageData(img) {
 *  canvas.width = img.width;
 *  canvas.height = img.height;
 *  ctx.drawImage(img, 0, 0);
 *  imageData = ctx.getImageData(0, 0, img.width, img.height);
 *  imageData = scale(imageData, canvas.width * expand_radius, canvas.height * expand_radius);
 *  imageData = NNExpand(imageData);
 *  canvas.width = imageData.width;
 *  canvas.height = imageData.height;
 *  ctx.putImageData(imageData, 0, 0);
 * }
 *
 * function NNExpand(base) {
 *   console.log(base);
 *   let img = new ImageData(expand_width, expand_height);
 *   let min_x = Math.floor(base.width / 2);
 *   let max_x = img.width - min_x - (base.width % 2);
 *   let min_y = Math.floor(base.height / 2);
 *   let max_y = img.height - min_y - (base.height % 2);
 *   console.log("Width: ", img.width);
 *   console.log("Height: ", img.height);
 *   console.log("min_x: ", min_x);
 *   console.log("max_x: ", max_x);
 *   console.log("min_y: ", min_y);
 *   console.log("max_y: ", max_y);
 *   console.log("Data: ", base.data);
 *   for(var i = 0; i < img.width; i++) {
 *     for(var j = 0; j < img.height; j++) {
 *       let x = i, y = j;
 *       if(x >= min_x && x < max_x) {
 *         x = min_x;
 *       } else if(x >= max_x) {
 *          x -= max_x;
 *          x += min_x;
 *       }
 *       if(y >= min_y && y < max_y) {
 *          y = min_y;
 *       } else if(y >= max_y) {
 *          y -= max_y;
 *          y += min_y;
 *       }
 *       img.data[4 * (i + j * img.width) + 0] = base.data[4 * (x + y * base.width) + 0];
 *       img.data[4 * (i + j * img.width) + 1] = base.data[4 * (x + y * base.width) + 1];
 *       img.data[4 * (i + j * img.width) + 2] = base.data[4 * (x + y * base.width) + 2];
 *       img.data[4 * (i + j * img.width) + 3] = base.data[4 * (x + y * base.width) + 3];
 *     }
 *   }
 *   console.log("Finished: ", img.data);
 *   return img;
 * }
 *
 * function scale(base, width, height) {
 *   let img = new ImageData(width, height);
 *   for(var i = 0; i < width * height; i++) {
 *     let x = Math.floor((i % width) * base.width / width),
 *         y = Math.floor(i  * base.width / width / width);  
 *     img.data[4 * i + 0] = base.data[4 * (x + y * base.width) + 0];
 *     img.data[4 * i + 1] = base.data[4 * (x + y * base.width) + 1];
 *     img.data[4 * i + 2] = base.data[4 * (x + y * base.width) + 2];
 *     img.data[4 * i + 3] = base.data[4 * (x + y * base.width) + 3];
 *   }
 *   return img;
 * }
 * </script>
 * @endhtmlonly
 *
 */
