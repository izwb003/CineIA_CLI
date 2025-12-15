% Generates unit test data

0;

vertices = csvread("vertices.csv");

num_vertices = rows(vertices);

% generate patches
 
mda_inv_basis_list = cell();

iab_basis_list = cell();

iab_n_list = cell();

iab_m_list = cell();

iab_q_list = cell();

patch_list = cell();

for i = 1 : num_vertices

	for j = i + 1 : num_vertices
	
		for k = j + 1 : num_vertices
		
		  v_mda_1 = vertices(i, 5:7);
  
			v_mda_2 = vertices(j, 5:7);
			
			v_mda_3 = vertices(k, 5:7);
			
			mda_basis = transpose([v_mda_1; v_mda_2; v_mda_3]);
			
			if (rcond(mda_basis) < 8 * eps)
				continue;
			endif
			
			mda_inv_basis = inv(mda_basis);
			
			is_patch = 1;
			
			for l = 1 : num_vertices
				
				if (sum(mda_inv_basis * transpose(vertices(l, 5:7))) > 1 + 8 * eps)
					
					is_patch = 0;
									
					break;
					
				endif
			
			endfor
			
			if (is_patch)
        
        v_iab_1 = vertices(i, 2:4);
  
        v_iab_2 = vertices(j, 2:4);
        
        v_iab_3 = vertices(k, 2:4);
        
        iab_basis = transpose([v_iab_1; v_iab_2; v_iab_3]);
        
        iab_n = cross(v_iab_2 - v_iab_1, v_iab_3 - v_iab_1);
        
        patch_list{end + 1} = [i, j, k];
        
        mda_inv_basis_list{end + 1} = mda_inv_basis;
        
        iab_basis_list{end + 1} = iab_basis;

        iab_n_list{end + 1} = iab_n;

        iab_q_list{end + 1} = dot(v_iab_1, iab_n);
			
				printf("Patch: %d, %d, %d\r\n", i - 1, j - 1, k - 1);
        
        printf("Patch: cond(mda) = %.12g, cond(iab) = %.12g\n", cond(mda_basis), cond(iab_basis));
			
			endif

		endfor
		
	endfor
	
endfor

num_patches = columns(mda_inv_basis_list);

printf("Number of patches: %d \r\n", num_patches);

fp = fopen("../src/main/cpp/mda_to_iab_coord_cvt_consts.h", "w");

fprintf(fp, "\r\n\
/*\r\n\
 * Generated using the pyra-mesa transform\r\n\
 */\r\n\
\r\n\
#include \"Matrix3.h\"\r\n\
#include \"Vector3.h\"\r\n\
\r\n");

fprintf(fp, "/* Number of pyra-mesa vertices */");
fprintf(fp, "const int num_vertices = %d;\r\n",  num_vertices);

fprintf(fp, "\r\n\
/*\r\n\
 * Coordinates of pyra-mesa vertices \r\n\
 * (iab_x, iab_y, iab_z, mda_x, mda_y, mda_z)\
 */\r\n\
\r\n");

fprintf(fp, "const float vertices[%d][6] = {\r\n", num_vertices);

for i = 1 : num_vertices
  
  fprintf(fp, "  /* Vertex %d at [%d, %d, %d] */\r\n\
          {%#.12gf, %#.12gf, %#.12gf, %#.12gf, %#.12gf, %#.12gf}",\
         i,\
         round(vertices(i, 2) * 100), round(vertices(i, 3) * 100), round(vertices(i, 4) * 100),\
         vertices(i, 2), vertices(i, 3), vertices(i, 4),\
         vertices(i, 5), vertices(i, 6), vertices(i, 7)\
         );
   
  if i == num_patches
    fprintf(fp, "\r\n");
  else
    fprintf(fp, ",\r\n");
  endif
  
endfor

fprintf(fp, "};\r\n");

fprintf(fp, "\r\n");
fprintf(fp, "/* Number of pyra-mesa patches */",  num_vertices);
fprintf(fp, "\r\n");

fprintf(fp, "const int num_patches = %d;\r\n",  num_patches);

fprintf(fp, "\r\n");

fprintf(fp, "\r\n");
fprintf(fp, "/* Inverse of the pyra-mesa patch basis matrix in the MDA coordinate system */");
fprintf(fp, "\r\n");
fprintf(fp, "const Matrix3 mda_inv_basis[%d] = {\r\n", num_patches);

for i = 1:num_patches
  
  fprintf(fp, "  {\r\n");
  
  fprintf(fp, "  /* Patch %d with vertices %d, %d, %d*/\r\n", i, patch_list{i}(1), patch_list{i}(2), patch_list{i}(3));
   
  fprintf(fp, "    {%#.12gf, %#.12gf, %#.12gf},\r\n", mda_inv_basis_list{i}(1, 1), mda_inv_basis_list{i}(2, 1), mda_inv_basis_list{i}(3, 1));
  fprintf(fp, "    {%#.12gf, %#.12gf, %#.12gf},\r\n", mda_inv_basis_list{i}(1, 2), mda_inv_basis_list{i}(2, 2), mda_inv_basis_list{i}(3, 2));
  fprintf(fp, "    {%#.12gf, %#.12gf, %#.12gf}\r\n", mda_inv_basis_list{i}(1, 3), mda_inv_basis_list{i}(2, 3), mda_inv_basis_list{i}(3, 3));
 
  if i == num_patches
    fprintf(fp, "  }\r\n");
  else
    fprintf(fp, "  },\r\n");
  endif
  
endfor
 
fprintf(fp, "};\r\n");

fprintf(fp, "\r\n");
fprintf(fp, "/* Pyra-mesa patch basis matrix in the IAB coordinate system */");
fprintf(fp, "\r\n");

fprintf(fp, "const Matrix3 iab_basis[%d] = {\r\n", num_patches);

for i = 1 : num_patches
  
  fprintf(fp, "  {\r\n");
  
  fprintf(fp, "  /* Patch %d with vertices %d, %d, %d*/\r\n", i, patch_list{i}(1), patch_list{i}(2), patch_list{i}(3));
   
  fprintf(fp, "    {%#.12gf, %#.12gf, %#.12gf},\r\n", iab_basis_list{i}(1, 1), iab_basis_list{i}(2, 1), iab_basis_list{i}(3, 1));
  fprintf(fp, "    {%#.12gf, %#.12gf, %#.12gf},\r\n", iab_basis_list{i}(1, 2), iab_basis_list{i}(2, 2), iab_basis_list{i}(3, 2));
  fprintf(fp, "    {%#.12gf, %#.12gf, %#.12gf}\r\n", iab_basis_list{i}(1, 3), iab_basis_list{i}(2, 3), iab_basis_list{i}(3, 3));
 
  if i == num_patches
    fprintf(fp, "  }\r\n");
  else
    fprintf(fp, "  },\r\n");
  endif
  
endfor
 
fprintf(fp, "};\r\n");

fprintf(fp, "\r\n");
fprintf(fp, "/* Normal vectors of pyra-mesa patches in the IAB coordinate system */");
fprintf(fp, "\r\n");

fprintf(fp, "const Vector3 iab_n[%d] = {\r\n", num_patches);

for i = 1:num_patches
    
  fprintf(fp, "\r\n  /* Patch %d with vertices %d, %d, %d*/\r\n", i, patch_list{i}(1), patch_list{i}(2), patch_list{i}(3));
  fprintf(fp, "  Vector3(%#.12gf, %#.12gf, %#.12gf)", iab_n_list{i}(1), iab_n_list{i}(2), iab_n_list{i}(3));
 
  if i == num_patches
    fprintf(fp, "\r\n");
  else
    fprintf(fp, ",\r\n");
  endif
  
endfor
 
fprintf(fp, "};\r\n");

fprintf(fp, "\r\n");
fprintf(fp, "/* Projection of a pyra-mesa patch basis onto its normal vector in the IAB coordinate system */");
fprintf(fp, "\r\n");

fprintf(fp, "const float iab_q[%d] = {\r\n", num_patches);

for i = 1:num_patches
  fprintf(fp, "\r\n");
  fprintf(fp, "  /* Patch %d with vertices %d, %d, %d*/\r\n", i, patch_list{i}(1), patch_list{i}(2), patch_list{i}(3));
  fprintf(fp, " %#.12gf", iab_q_list{i});
 
  if i == num_patches
    fprintf(fp, "\r\n");
  else
    fprintf(fp, ",\r\n");
  endif
  
endfor
 
fprintf(fp, "};\r\n");

fclose(fp);