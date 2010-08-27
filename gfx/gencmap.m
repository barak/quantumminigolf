%gencmap -- an octave script to generate the colormap bitmap

[x, y] = meshgrid(linspace(-1, 1, 256));

psi = x + i*y;

colormap(gray(256));
intens = imagesc(min(abs(psi), 1));
saveimage("intens.ppm", intens, "ppm");

colormap(hsv2rgb([linspace(0, 1, 256)', ones(256, 1), ones(256, 1)]));
color = imagesc((arg(psi) + pi)/2/pi);
saveimage("color.ppm", color, "ppm");

% now start gimp and merge the two bitmaps by hand
