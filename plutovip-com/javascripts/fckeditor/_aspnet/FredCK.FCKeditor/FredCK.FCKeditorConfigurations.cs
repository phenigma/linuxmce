using System ;
using System.Collections ;

namespace FredCK
{
	public class FCKeditorConfigurations
	{
		private Hashtable colConfigs ;

		internal FCKeditorConfigurations()
		{
			colConfigs = new Hashtable() ;
		}

		public string this[ string configurationName ]
		{
			get
			{
				if ( colConfigs.ContainsKey( configurationName ) )
					return (string)colConfigs[ configurationName ] ;
				else
					return null ;
			}
			set
			{
				colConfigs[ configurationName ] = value ;
			}
		}

		internal string GetUrlParams()
		{
			System.Text.StringBuilder osParams = new System.Text.StringBuilder() ;

			foreach ( DictionaryEntry oEntry in colConfigs )
			{
				osParams.AppendFormat( "&{0}={1}", oEntry.Key, oEntry.Value ) ;
			}

			return osParams.ToString() ;
		}
	}
}
