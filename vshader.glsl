attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec4 vColor;

varying vec3 fN;
varying vec3 fV;
varying vec3 fL;

varying vec4 color;

uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform mat4 Projection;

uniform int flag;
varying vec3 texCoord;
attribute vec3 vTexCoord;
uniform int phong;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct,colorDefiner;
uniform float Shininess;

void main(){
    
    if(phong == 1){
    
        texCoord = vTexCoord;
    
        fN = (ModelView*vec4(vNormal, 0.0)).xyz;
    
        fV = (ModelView * vPosition).xyz;
    
        fL = LightPosition.xyz;
    
        if( LightPosition.w != 0.0 ) {
            fL = LightPosition.xyz - fV;
        }
    
        color = vColor;
    
        gl_Position = Projection*ModelView*vPosition;
        
    } else {        
        vec3 pos = (ModelView * vPosition).xyz;
        
        vec3 L = normalize( LightPosition.xyz - pos );
        vec3 V = normalize( -pos );
        vec3 H = normalize( L + V ); // halfway vector
        
        vec3 N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;
        
        vec4 ambient = AmbientProduct;
        
        float Kd = max( dot(L, N), 0.0 ); //set diffuse to 0 if light is behind the surface point
        vec4  diffuse = Kd*DiffuseProduct;
        
        float Ks = pow( max(dot(N, H), 0.0), Shininess );
        vec4  specular = Ks * SpecularProduct;
        
        if( dot(L, N) < 0.0 ) {
            specular = vec4(0.0, 0.0, 0.0, 1.0);
        }
        
        gl_Position = Projection * ModelView * vPosition;
        
        color = ambient + diffuse + specular;
        color.a = 1.0;  
    }
}