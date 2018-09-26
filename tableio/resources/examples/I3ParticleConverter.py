"""A demonstration of a converter written in pure Python"""

from icecube import tableio,dataclasses

class I3ParticleConverter(tableio.I3Converter):
	booked = dataclasses.I3Particle
	def CreateDescription(self,part):
		desc = tableio.I3TableRowDescription()
		desc.add_field('x',      tableio.types.Float64,'m',     'x-position of particle')
		desc.add_field('y',      tableio.types.Float64,'m',     'y-position of particle')
		desc.add_field('z',      tableio.types.Float64,'m',     'z-position of particle')
		desc.add_field('time',   tableio.types.Float64,'ns',    'origin time of particle')
		desc.add_field('zenith', tableio.types.Float64,'radian','zenith angle of particle origin')
		desc.add_field('azimuth',tableio.types.Float64,'radian','azimuthal angle of particle origin')
		desc.add_field('energy', tableio.types.Float64,'GeV',   'energy of particle')
		desc.add_field('speed',  tableio.types.Float64,'Gm/s',  'particle speed')
		desc.add_field('length', tableio.types.Float64,'m',     'length of track')
		# skip: major_id/minor_id
		desc.add_field('type',      tableio.I3Datatype(self.booked.ParticleType),'','')
		desc.add_field('shape',     tableio.I3Datatype(self.booked.ParticleShape),'','')
		desc.add_field('location',  tableio.I3Datatype(self.booked.LocationType),'','')
		desc.add_field('fit_status',tableio.I3Datatype(self.booked.FitStatus),'','')
		return desc
	def Convert(self,particle,row,frame):
		row['x']          = particle.x
		row['y']          = particle.y
		row['z']          = particle.z
		row['time']       = particle.time
		row['zenith']     = particle.zenith
		row['azimuth']    = particle.azimuth
		row['energy']     = particle.energy
		row['speed']      = particle.speed
		row['length']     = particle.length
		row['type']       = particle.type
		row['shape']      = particle.shape
		row['location']   = particle.location_type
		row['fit_status'] = particle.fit_status
		return 1
		
# tableio.I3ConverterRegistry.register(I3ParticleConverter)
