varying  vec3 fN;
varying  vec3 fL;
varying  vec3 fV;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform float Shininess;

varying vec3 texCoord;
varying vec4 color;

uniform int phong;
uniform int flag;
uniform vec4 colorDefiner;
uniform sampler2D texture;

void main(){
    
    vec3 N = normalize(fN);
    vec3 V = normalize(fV);
    vec3 L = normalize(fL);
    
    vec3 H = normalize( L + V );
    
    vec4 ambient = AmbientProduct;
    
    float Kd = max(dot(L, N), 0.0);
    vec4 diffuse = Kd*DiffuseProduct;
    
    float Ks = pow(max(dot(N, H), 0.0), Shininess);
    vec4 specular = Ks*SpecularProduct;
    
    if(dot(L, N) < 0.0 ) {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    }
    vec2 longitudeLatitude = vec2((atan(texCoord.y, texCoord.x) / 3.1415926 + 1.0) * 0.5, (asin(texCoord.z) / 3.1415926 + 0.5)); 
    
    if(flag == 0){
        if(phong == 1){
            gl_FragColor = (ambient + diffuse + specular)*texture2D( texture, longitudeLatitude)*color*colorDefiner;
            gl_FragColor.a = 1.0;
        }else{
            gl_FragColor = colorDefiner*color;
        }
    } else if(flag == 1) { 
        if(phong == 1) {
            gl_FragColor = (ambient + diffuse + specular)*color*colorDefiner;
            gl_FragColor.a = 1.0;
        }else {
            gl_FragColor = colorDefiner*color;
        }
    } else if(flag == 2) { 
        if(phong == 1) {
            gl_FragColor = colorDefiner;
        } else {
            gl_FragColor = color;
        }   
    }
}