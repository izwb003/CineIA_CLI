% Generates unit test data

0;
function r = fpatch (p, a)
    
  for i = 1:rows(a)
    
    t = a{i} * p;
    
%    printf("%d, %f, %f, %f\r\n",  i,t(1), t(2), t(3)); 
    
    if (t(1) >= 0 && t(2) >= 0 && t(3) >= 0)
      r = i;
      return;
    endif
  
  endfor
  
  r = -1;
  
endfunction

vertices = csvread("vertices.csv");
patches = csvread("patches.csv");

a = cell(rows(patches), 1);

b = cell(rows(patches), 1);

n = cell(rows(patches), 1);

q = cell(rows(patches), 1);

for i = 1:rows(patches)
  
  v1i = patches(i, 2) + 1;
  
  v2i = patches(i, 3) + 1;
  
  v3i = patches(i, 4) + 1;
  
  v1 = vertices(v1i, 2:4);
  
  v2 = vertices(v2i, 2:4);
  
  v3 = vertices(v3i, 2:4);
  
  a{i} = inv(transpose([v1; v2; v3]));
  
  n{i} = cross(v2 - v1, v3 - v1);
  
  n{i} = n{i}/norm(n{i});
  
  q{i} = dot(v1, n{i});
  
  b{i} = transpose([vertices(v1i, 5:7); vertices(v2i,5:7); vertices(v3i, 5:7)]);
  
endfor

fp = fopen("IABTOMDAPOSTRANSFORMTESTS.h", "w");

fprintf(fp, "\r\n\
/*\r\n\
 * IABTOMDAPOSTRANSFORMTESTS\r\n\
 * \r\n\
 * {iab_x, iab_y, iab_z, mda_x, mda_y, mda_z}\r\n\
 *\r\n\
 * Generated using the pyra-mesa transform\r\n\
 */\r\n\
");

fprintf(fp, "float IABTOMDAPOSTRANSFORMTESTS[][] = { \r\n");

for i = 0:0.125:1
  
  for j = 0:0.125:1
  
    for k = 0:0.125:1
      
      if (i == 0.5 && j == 0.5 && k == 0)
        
        continue;
        
      endif
      
      o = [2*i - 1; 1 - 2*j; k];
      
      pidx = fpatch(o, a);
      
      w = b{pidx} * a{pidx} * o;
      
      w = w / norm(w) * dot(o, n{pidx}) / q{pidx};
      
      if (pidx == -1) 
      
        printf("Incomplete convex hull: %f, %f, %f\r\n",  2*i - 1, 2*j - 1, k);
       
        return; 
    
      endif
    
      fprintf(fp, "   {%f, %f, %f, %f, %f, %f}", i, j, k, w(1), w(2), w(3)); 
      
      if (! (i == 1 && j == 1 && k == 1))
        fprintf(fp, ",");
      endif
      
      fprintf(fp, "\r\n");
    
    endfor
  
  endfor
  
endfor

fprintf(fp, "   };\r\n");

fclose(fp);