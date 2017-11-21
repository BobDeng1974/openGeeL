
width 	= 1920
height 	= 1080
name    = "DeerScene"
mode 	= "Windowed"

usePhysics  = false
useEmissive = false
exposure = 1

camera = {
	position = { x = -0.03,  y = 0.17, z = 2.66 },
	rotation = { x = -91.59, y = 2.78, z = -3 },

	fov = 25,
	near = 0.01,
	far = 100,

	movable = true,
	speed = 1,
	sensitivity = 0.45

}

skybox = {
	path = "resources/envmaps/WinterForest_Ref.hdr",
	resolution = 1024,

	useIBL = true,
	drawIrradiance = false

}

reflectionprobes = {
	
	{
		position 	= { x = -0.13, 	y = 0.13, 	 z = 0.52 },
		rotation 	= { x = 0,  	y = 0,   	 z = 0 },
		dimensions 	= { width = 20, height = 20, depth = 20},

		resolution = 1024
	},

}


meshes = {
	
	{
		position = { x = 0, 	y = 0, 	  z = 0 },
		rotation = { x = 0,  	y =  0,   z = 0 },
		scale 	 = { x = 0.1, 	y =  0.1, z =  0.1 },

		path = "resources/deer/scene2.obj",
		name = "Deer"
	},

}


lights = {
	
	{
		type = "Point",

		position = { x = -0.29, y = 0.39, z = 1.8 },
		rotation = { x = -180, 	y = 0, 	  z = -50},
		color	 = { r = 1, 	g = 1, 	  b = 1},

		intensity = 10,

		useShadowmap = true,
		shadowBias = 0.00001,
		shadowResolution = "Huge",
		shadowmapType = "Soft",
		softShadowResolution = 15
	}

}


ibl = {}

ssao = {
	radius = 0.5,
	blurSigma = 4,
	blurFactor = 0.291
}

fxaa = {
	minColorDiff = 0,
	fxaaMul = 0

}


gui = {
	
	showObjects = true,
	showEffects = true,
	showSystem  = true

}
